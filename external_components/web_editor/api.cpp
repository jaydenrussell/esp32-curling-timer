#include "api.h"

#include <cstdio>
#include <cstring>
#include <ctime>

#include "esphome/components/curling_engine/config_edit.h"
#include "esphome/components/curling_engine/model.h"
#include "esphome/components/curling_engine/quickstart.h"
#include "json_model.h"

namespace esphome {
namespace web_editor {

using curling_engine::ConfigValue;
using curling_engine::DAY_NAMES;
using curling_engine::Entry;
using curling_engine::ExceptionDay;
using curling_engine::PiTimerSettings;
using curling_engine::ScheduleModel;
using curling_engine::SettingsStore;
using curling_engine::StoreStatus;

static constexpr const char *SESSION_COOKIE_NAME = "pitimer_session";
static constexpr const char *INVALID_SETTINGS_PREFIX =
    "the live settings file is invalid, fix it in the Raw YAML tab first: ";
static constexpr const char *VALID_RELAY_NAMES[] = {"RELAY1", "RELAY2", "RELAY3", "RELAY4"};

// -- small helpers ------------------------------------------------------------

static ApiResponse error_json(int status, const std::string &message) {
  JsonDocument doc;
  doc["error"] = message;
  ApiResponse resp;
  resp.status = status;
  serializeJson(doc, resp.body);
  return resp;
}

static ApiResponse ok_json() {
  JsonDocument doc;
  doc["ok"] = true;
  ApiResponse resp;
  serializeJson(doc, resp.body);
  return resp;
}

static ApiResponse from_document(JsonDocument &doc, int status = 200) {
  ApiResponse resp;
  resp.status = status;
  serializeJson(doc, resp.body);
  return resp;
}

static ApiResponse store_error(StoreStatus status, const std::string &err) {
  switch (status) {
    case StoreStatus::CONFLICT:
      return error_json(409, err);
    case StoreStatus::INVALID:
      return error_json(422, err);
    default:
      return error_json(500, err);
  }
}

static std::string first_line(const std::string &text) {
  const size_t nl = text.find('\n');
  std::string line = nl == std::string::npos ? text : text.substr(0, nl);
  if (!line.empty() && line.back() == '\r')
    line.pop_back();
  return line;
}

static std::string trim_spaces(const std::string &text) {
  size_t begin = 0, end = text.size();
  while (begin < end && text[begin] == ' ')
    begin++;
  while (end > begin && text[end - 1] == ' ')
    end--;
  return text.substr(begin, end - begin);
}

static bool content_type_is_json(const std::string &header) {
  if (header.empty())
    return false;
  const size_t semi = header.find(';');
  std::string kind = header.substr(0, semi == std::string::npos ? std::string::npos : semi);
  while (!kind.empty() && (kind.back() == ' ' || kind.back() == '\t'))
    kind.pop_back();
  return kind == "application/json";
}

// python urlsplit(origin).netloc
static std::string origin_netloc(const std::string &origin) {
  const size_t scheme = origin.find("://");
  std::string rest = scheme == std::string::npos ? origin : origin.substr(scheme + 3);
  size_t cut = rest.size();
  for (char sep : {'/', '?', '#'}) {
    const size_t pos = rest.find(sep);
    if (pos != std::string::npos && pos < cut)
      cut = pos;
  }
  return rest.substr(0, cut);
}

static bool origin_ok(const std::string &origin, const std::string &host) {
  if (origin.empty())
    return true;
  return origin_netloc(origin) == host;
}

// python urllib.parse.unquote for the backup-name path suffix
static std::string unquote_path(const std::string &text) {
  auto nib = [](char c) -> int {
    if (c >= '0' && c <= '9')
      return c - '0';
    if (c >= 'a' && c <= 'f')
      return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
      return c - 'A' + 10;
    return -1;
  };
  std::string out;
  out.reserve(text.size());
  for (size_t i = 0; i < text.size(); i++) {
    if (text[i] == '%' && i + 2 < text.size()) {
      const int h = nib(text[i + 1]), l = nib(text[i + 2]);
      if (h >= 0 && l >= 0) {
        out.push_back(static_cast<char>(h * 16 + l));
        i += 2;
        continue;
      }
    }
    out.push_back(text[i]);
  }
  return out;
}

static bool session_from_cookie(const std::string &cookie, std::string *value) {
  const std::string key = std::string(SESSION_COOKIE_NAME) + "=";
  size_t pos = 0;
  while (pos < cookie.size()) {
    while (pos < cookie.size() && (cookie[pos] == ' ' || cookie[pos] == ';' || cookie[pos] == '\t'))
      pos++;
    if (cookie.compare(pos, key.size(), key) == 0) {
      const size_t start = pos + key.size();
      size_t end = cookie.find(';', start);
      if (end == std::string::npos)
        end = cookie.size();
      *value = cookie.substr(start, end - start);
      return true;
    }
    const size_t next = cookie.find(';', pos);
    if (next == std::string::npos)
      break;
    pos = next + 1;
  }
  return false;
}

static std::string session_header(const std::string &value, long max_age) {
  char buf[16];
  std::snprintf(buf, sizeof(buf), "%ld", max_age);
  return std::string(SESSION_COOKIE_NAME) + "=" + value + "; Max-Age=" + buf +
         "; Path=/; HttpOnly; SameSite=Strict";
}

static bool json_strict_int(JsonVariantConst v, long long *out) {
  if (v.isNull() || v.is<bool>() || !v.is<long long>())
    return false;
  if (v.is<double>()) {
    const double d = v.as<double>();
    if (d != static_cast<double>(v.as<long long>()))
      return false;
  }
  *out = v.as<long long>();
  return true;
}

static bool version_arg(JsonObjectConst data, std::string *version, ApiResponse *bad) {
  JsonVariantConst v = data["version"];
  if (!v.is<const char *>() || v.as<const char *>()[0] == '\0') {
    *bad = error_json(400, "'version' is required, reload the page");
    return false;
  }
  *version = v.as<const char *>();
  return true;
}

static bool text_arg(JsonObjectConst data, std::string *text, ApiResponse *bad) {
  JsonVariantConst v = data["text"];
  if (!v.is<const char *>()) {
    *bad = error_json(400, "'text' is required");
    return false;
  }
  *text = v.as<const char *>();
  return true;
}

// -- live file ---------------------------------------------------------------

bool PiTimerApi::parsed_live(ParsedLive *out) {
  SettingsStore *store = engine_->get_store();
  if (store == nullptr) {
    out->error = "settings store not ready";
    return false;
  }
  std::string text, version, err;
  if (store->read(&text, &version, &err) != StoreStatus::OK) {
    out->error = first_line(err);
    return false;
  }
  out->version = std::move(version);
  out->valid = out->settings.parse_settings_text(text, curling_engine::SETTINGS_BASENAME, &err);
  if (!out->valid)
    out->error = first_line(err);
  out->model_ok = curling_engine::load_model(text, &out->model, &err);
  return true;
}

bool PiTimerApi::valid_settings(ParsedLive *live, ApiResponse *resp) {
  if (!parsed_live(live) || !live->valid) {
    *resp = error_json(422, std::string(INVALID_SETTINGS_PREFIX) + live->error);
    return false;
  }
  return true;
}

bool PiTimerApi::logged_in(const std::string &cookie) const {
  std::string session;
  if (!session_from_cookie(cookie, &session))
    return false;
  return config_->session_valid(session, static_cast<float>(std::time(nullptr)));
}

bool PiTimerApi::now_ok(int *weekday_mon0, int *seconds_of_day, std::string *date_iso) const {
  return engine_->local_now(weekday_mon0, seconds_of_day, date_iso);
}

// -- status / settings -------------------------------------------------------

ApiResponse PiTimerApi::status() {
  ParsedLive live;
  parsed_live(&live);
  JsonDocument doc;
  doc["version"] = live.version;
  doc["valid"] = live.valid;
  doc["error"] = live.error;
  int weekday = -1, sec = -1;
  std::string date_iso;
  const bool clock_ok = now_ok(&weekday, &sec, &date_iso);
  doc["clock_synced"] = clock_ok;
  if (live.valid) {
    doc["enabled"] = live.settings.enabled;
    doc["header_hold"] = live.settings.relay_msg_time;
  }
  if (!clock_ok || !live.valid || !live.model_ok)
    return from_document(doc);
  char now_buf[40];
  std::snprintf(now_buf, sizeof(now_buf), "%s %02d:%02d:%02d", date_iso.c_str(), sec / 3600,
                (sec % 3600) / 60, sec % 60);
  doc["now"] = now_buf;
  doc["weekday"] = DAY_NAMES[weekday];
  const std::vector<Entry> entries = live.model.entries_for(date_iso);
  const std::optional<curling_engine::PiTimerGame> game =
      live.settings.find_current_game(weekday, sec, date_iso);
  if (game.has_value()) {
    JsonObject current = doc["current"].to<JsonObject>();
    if (game->message.has_value())
      current["name"] = *game->message;
    else
      current["name"] = nullptr;
    current["start"] = game->start;
    current["buzzer"] = game->buzzer;
    if (game->seconds_until_buzzer.has_value())
      current["seconds_until_buzzer"] = *game->seconds_until_buzzer;
    else
      current["seconds_until_buzzer"] = nullptr;
    bool quick = false;
    for (const Entry &entry : entries) {
      if (entry.start == game->start && entry.quick) {
        quick = true;
        break;
      }
    }
    current["quick"] = quick;
  } else {
    doc["current"] = nullptr;
  }
  const Entry *upcoming = nullptr;
  for (const Entry &entry : entries) {
    if (entry.start > sec && (upcoming == nullptr || entry.start < upcoming->start))
      upcoming = &entry;
  }
  if (upcoming != nullptr)
    entry_into_json(doc["next"].to<JsonObject>(), *upcoming);
  else
    doc["next"] = nullptr;
  const ExceptionDay *exception = live.model.exception_for(date_iso);
  JsonObject today = doc["today"].to<JsonObject>();
  today["source"] = exception != nullptr ? "exception" : "weekday";
  today["note"] = exception != nullptr ? exception->note : "";
  JsonArray today_entries = today["entries"].to<JsonArray>();
  for (const Entry &entry : entries)
    add_entry_json(today_entries, entry);
  return from_document(doc);
}

ApiResponse PiTimerApi::get_settings() {
  SettingsStore *store = engine_->get_store();
  std::string text, version, err;
  if (store == nullptr || store->read(&text, &version, &err) != StoreStatus::OK)
    return error_json(500, first_line(err));
  ScheduleModel model;
  if (!curling_engine::load_model(text, &model, &err))
    return error_json(422, err + " (use the Raw YAML tab to fix the file)");
  JsonDocument doc;
  doc["version"] = version;
  JsonDocument model_doc;
  if (deserializeJson(model_doc, model_to_json(model)))
    return error_json(500, "could not encode the settings model");
  JsonObjectConst packed = model_doc.as<JsonObjectConst>();
  doc["schedule"] = packed["schedule"];
  doc["exceptions"] = packed["exceptions"];
  doc["inner_comments"] = packed["inner_comments"];
  return from_document(doc);
}

ApiResponse PiTimerApi::put_settings(JsonObjectConst data) {
  ScheduleModel model;
  std::string err;
  if (!model_from_json(data, &model, &err))
    return error_json(400, err);
  std::string version;
  ApiResponse bad;
  if (!version_arg(data, &version, &bad))
    return bad;
  SettingsStore *store = engine_->get_store();
  std::string new_version;
  const StoreStatus status = store->update(
      [&model](const std::string &text, std::string *changed, std::string *cerr) {
        ScheduleModel live;
        if (!curling_engine::load_model(text, &live, cerr))
          return false;
        return curling_engine::splice(text, model, changed, cerr);
      },
      &version, &new_version, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  doc["ok"] = true;
  doc["version"] = new_version;
  return from_document(doc);
}

ApiResponse PiTimerApi::set_enabled(JsonObjectConst data) {
  JsonVariantConst enabled = data["enabled"];
  if (!enabled.is<bool>())
    return error_json(400, "'enabled' must be true or false");
  const bool want = enabled.as<bool>();
  std::string new_version, err;
  SettingsStore *store = engine_->get_store();
  const StoreStatus status = store->update(
      [want](const std::string &text, std::string *changed, std::string *cerr) {
        return curling_engine::set_enabled(text, want, changed, cerr);
      },
      nullptr, &new_version, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  doc["ok"] = true;
  doc["version"] = new_version;
  return from_document(doc);
}

ApiResponse PiTimerApi::get_raw() {
  SettingsStore *store = engine_->get_store();
  std::string text, version, err;
  if (store == nullptr || store->read(&text, &version, &err) != StoreStatus::OK)
    return error_json(500, first_line(err));
  JsonDocument doc;
  doc["version"] = version;
  doc["text"] = text;
  return from_document(doc);
}

ApiResponse PiTimerApi::put_raw(JsonObjectConst data) {
  std::string text;
  ApiResponse bad;
  if (!text_arg(data, &text, &bad))
    return bad;
  std::string version;
  if (!version_arg(data, &version, &bad))
    return bad;
  std::string new_version, err;
  const StoreStatus status = engine_->get_store()->save(text, &version, &new_version, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  doc["ok"] = true;
  doc["version"] = new_version;
  return from_document(doc);
}

ApiResponse PiTimerApi::validate_raw(JsonObjectConst data) {
  std::string text;
  ApiResponse bad;
  if (!text_arg(data, &text, &bad))
    return bad;
  std::string err;
  const StoreStatus status = engine_->get_store()->validate(text, &err);
  if (status == StoreStatus::INVALID) {
    JsonDocument doc;
    doc["ok"] = false;
    doc["error"] = err;
    return from_document(doc);
  }
  if (status != StoreStatus::OK)
    return store_error(status, err);
  return ok_json();
}

ApiResponse PiTimerApi::get_backups() {
  std::vector<curling_engine::BackupInfo> backups;
  std::string err;
  const StoreStatus status = engine_->get_store()->backups(&backups, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  JsonArray array = doc["backups"].to<JsonArray>();
  for (const curling_engine::BackupInfo &info : backups) {
    JsonObject obj = array.add<JsonObject>();
    obj["name"] = info.name;
    obj["size"] = info.size;
    obj["mtime"] = info.mtime;
  }
  return from_document(doc);
}

ApiResponse PiTimerApi::get_backup(const std::string &name) {
  std::string text, err;
  const StoreStatus status = engine_->get_store()->read_backup(name, &text, &err);
  if (status != StoreStatus::OK)
    return error_json(404, err);
  JsonDocument doc;
  doc["name"] = name;
  doc["text"] = text;
  return from_document(doc);
}

ApiResponse PiTimerApi::restore(JsonObjectConst data) {
  JsonVariantConst name_v = data["name"];
  if (!name_v.is<const char *>())
    return error_json(400, "'name' is required");
  const std::string name = name_v.as<const char *>();
  std::string text, err;
  const StoreStatus read_status = engine_->get_store()->read_backup(name, &text, &err);
  if (read_status != StoreStatus::OK)
    return error_json(404, err);
  std::string version;
  ApiResponse bad;
  if (!version_arg(data, &version, &bad))
    return bad;
  std::string new_version;
  const StoreStatus status = engine_->get_store()->save(text, &version, &new_version, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  doc["ok"] = true;
  doc["version"] = new_version;
  return from_document(doc);
}

// -- quick start --------------------------------------------------------------

ApiResponse PiTimerApi::quick_start(JsonObjectConst data) {
  int weekday = -1, sec = -1;
  std::string date_iso;
  if (!now_ok(&weekday, &sec, &date_iso))
    return error_json(503, "clock not synced yet");
  long long seconds;
  if (!json_strict_int(data["seconds"], &seconds))
    return error_json(400, "'seconds' must be a whole number");
  std::optional<std::string> name, during_buzzer;
  if (!data["name"].isNull()) {
    if (!data["name"].is<const char *>())
      return error_json(400, "'name' must be text");
    const std::string value = trim_spaces(data["name"].as<const char *>());
    if (!value.empty())
      name = value;
  }
  if (!data["during_buzzer"].isNull()) {
    if (!data["during_buzzer"].is<const char *>())
      return error_json(400, "'during_buzzer' must be text");
    const std::string value = trim_spaces(data["during_buzzer"].as<const char *>());
    if (!value.empty())
      during_buzzer = value;
  }
  std::optional<int> header_hold;
  if (!data["header_hold"].isNull()) {
    long long value;
    if (!json_strict_int(data["header_hold"], &value) || value < 0)
      return error_json(400, "'header_hold' must be a whole number of seconds (0 or more)");
    header_hold = static_cast<int>(value);
  }
  ParsedLive live;
  ApiResponse bad;
  if (!valid_settings(&live, &bad))
    return bad;
  std::string version;
  if (!version_arg(data, &version, &bad))
    return bad;
  curling_engine::QuickStartOptions opts;
  opts.date_iso = date_iso;
  opts.now_sec = sec;
  opts.duration = static_cast<int>(seconds);
  opts.name = name;
  opts.pace_of_play = data["pace_of_play"].is<bool>() && data["pace_of_play"].as<bool>();
  opts.replace_conflicts = data["replace_conflicts"].is<bool>() && data["replace_conflicts"].as<bool>();
  opts.during_buzzer = during_buzzer;
  opts.header_hold = header_hold;
  opts.hold = live.settings.relay_msg_time;
  opts.pregame_countdown = live.settings.pregame_countdown;
  Entry added;
  std::string new_version, err;
  SettingsStore *store = engine_->get_store();
  const StoreStatus status = store->update(
      [&opts, &added](const std::string &text, std::string *changed, std::string *cerr) {
        ScheduleModel model;
        if (!curling_engine::load_model(text, &model, cerr))
          return false;
        if (!curling_engine::quick_start(&model, opts, &added, cerr))
          return false;
        return curling_engine::splice(text, model, changed, cerr);
      },
      &version, &new_version, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  doc["ok"] = true;
  doc["version"] = new_version;
  entry_into_json(doc["entry"].to<JsonObject>(), added);
  doc["buzzer_time"] = trim_spaces(curling_engine::format_time(added.buzzer));
  return from_document(doc);
}

ApiResponse PiTimerApi::cancel_quick_start(JsonObjectConst data) {
  int weekday = -1, sec = -1;
  std::string date_iso;
  if (!now_ok(&weekday, &sec, &date_iso))
    return error_json(503, "clock not synced yet");
  long long start;
  if (!json_strict_int(data["start"], &start))
    return error_json(400, "'start' must be a number of seconds");
  std::string new_version, err;
  const StoreStatus status = engine_->get_store()->update(
      [&date_iso, start](const std::string &text, std::string *changed, std::string *cerr) {
        ScheduleModel model;
        if (!curling_engine::load_model(text, &model, cerr))
          return false;
        if (!curling_engine::cancel_quick_start(&model, date_iso, static_cast<int>(start), cerr))
          return false;
        return curling_engine::splice(text, model, changed, cerr);
      },
      nullptr, &new_version, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  doc["ok"] = true;
  doc["version"] = new_version;
  return from_document(doc);
}

ApiResponse PiTimerApi::test_relay(JsonObjectConst data) {
  const char *ev = data["event"].as<const char *>();
  std::string event = ev ? ev : "";
  if (event != "Game End" && event != "Game Start" && event != "Countdown Warning")
    return error_json(400, "invalid event");
  ParsedLive live;
  ApiResponse bad;
  if (!valid_settings(&live, &bad))
    return bad;
  const PiTimerSettings &s = live.settings;
  std::vector<std::string> relays;
  if (event == "Game End" && s.relays.has_value())
    relays = *s.relays;
  else if (event == "Game Start" && s.ring_at_start.has_value())
    relays = *s.ring_at_start;
  else if (event == "Countdown Warning")
    relays = s.countdown_relays;
  JsonDocument doc;
  doc["ok"] = true;
  doc["event"] = event;
  JsonArray arr = doc["relays"].to<JsonArray>();
  for (const auto &r : relays) arr.add(r);
  doc["duration"] = s.relay_duration;
  return from_document(doc);
}

// -- config (new, no python parity) -------------------------------------------

ApiResponse PiTimerApi::get_config() {
  ParsedLive live;
  ApiResponse bad;
  if (!valid_settings(&live, &bad))
    return bad;
  const PiTimerSettings &s = live.settings;
  JsonDocument doc;
  doc["version"] = live.version;
  JsonObject cfg = doc["config"].to<JsonObject>();
  cfg["config.enabled"] = s.enabled;
  cfg["config.header.font_size"] = s.font_sizes.count("header") != 0 ? s.font_sizes.at("header") : 0;
  for (const char *event : {"when_disabled", "when_idle", "counting_down", "during_buzzer"}) {
    for (const char *attribute : {"text", "color"}) {
      std::string key = std::string("config.header.") + attribute + "." + event;
      auto group = s.headers.find(attribute);
      bool present = false;
      if (group != s.headers.end()) {
        auto value = group->second.find(event);
        if (value != group->second.end() && !value->second.empty()) {
          cfg[key] = value->second;
          present = true;
        }
      }
      if (!present)
        cfg[key] = nullptr;
    }
  }
  cfg["config.countdown.font_size_4_digits"] =
      s.font_sizes.count("timer_4_digits") != 0 ? s.font_sizes.at("timer_4_digits") : 0;
  cfg["config.countdown.font_size_5_digits"] =
      s.font_sizes.count("timer_5_digits") != 0 ? s.font_sizes.at("timer_5_digits") : 0;
  auto countdown_color = [&](const char *event) {
    std::string key = std::string("config.countdown.color.") + event;
    auto it = s.countdown_colors.find(event);
    if (it != s.countdown_colors.end() && !it->second.empty())
      cfg[key] = it->second;
    else
      cfg[key] = nullptr;
  };
  countdown_color("counting_down");
  countdown_color("during_buzzer");
  cfg["config.pace_of_play.enabled"] = s.pace_of_play;
  cfg["config.pace_of_play.font_size"] =
      s.font_sizes.count("pace_of_play") != 0 ? s.font_sizes.at("pace_of_play") : 0;
  cfg["config.pace_of_play.color"] = s.pace_color;
  cfg["config.pace_of_play.unit_name"] = s.pace_unit_name;
  cfg["config.pace_of_play.unit_duration"] = s.pace_unit_duration;
  cfg["config.footer.font_size"] = s.font_sizes.count("footer") != 0 ? s.font_sizes.at("footer") : 0;
  cfg["config.footer.datetime"] = s.date_and_time;
  cfg["config.footer.entry_name"] = s.game_name;
  if (s.relays.has_value()) {
    JsonArray relays = cfg["config.buzzer.relays"].to<JsonArray>();
    for (const std::string &relay : *s.relays)
      relays.add(relay);
  } else {
    cfg["config.buzzer.relays"] = nullptr;
  }
  if (s.ring_at_start.has_value()) {
    JsonArray relays = cfg["config.buzzer.ring_at_start"].to<JsonArray>();
    for (const std::string &relay : *s.ring_at_start)
      relays.add(relay);
  } else {
    cfg["config.buzzer.ring_at_start"] = nullptr;
  }
  cfg["config.buzzer.countdown.minutes_before"] = s.countdown_minutes_before;
  {
    JsonArray relays = cfg["config.buzzer.countdown.relays"].to<JsonArray>();
    for (const std::string &relay : s.countdown_relays)
      relays.add(relay);
  }
  cfg["config.buzzer.duration"] = s.relay_duration;
  cfg["config.buzzer.header_hold"] = s.relay_msg_time;
  cfg["config.relays_enabled"] = s.relays_enabled;
  cfg["config.relay_count"] = s.relay_count;
  cfg["config.relay1_function"] = s.relay1_function;
  cfg["config.relay2_function"] = s.relay2_function;
  cfg["config.relay3_function"] = s.relay3_function;
  cfg["config.relay4_function"] = s.relay4_function;
  cfg["config.relay1_enabled"] = s.relay1_enabled;
  cfg["config.relay2_enabled"] = s.relay2_enabled;
  cfg["config.relay3_enabled"] = s.relay3_enabled;
  cfg["config.relay4_enabled"] = s.relay4_enabled;
  cfg["config.pregame_countdown"] = s.pregame_countdown;
  return from_document(doc);
}

ApiResponse PiTimerApi::post_config(JsonObjectConst data) {
  JsonVariantConst cfg_v = data["config"];
  if (!cfg_v.is<JsonObjectConst>())
    return error_json(400, "'config' is required");
  std::string version;
  ApiResponse bad;
  if (!version_arg(data, &version, &bad))
    return bad;
  JsonObjectConst cfg = cfg_v.as<JsonObjectConst>();
  std::vector<ConfigValue> values;
  for (JsonPairConst kv : cfg) {
    const std::string path = kv.key().c_str();
    JsonVariantConst v = kv.value();
    std::string yaml;
    long long number = 0;
    if (path == "config.enabled" || path == "config.pace_of_play.enabled" ||
        path == "config.footer.datetime" || path == "config.footer.entry_name" ||
        path == "config.relays_enabled" || path == "config.relay1_enabled" ||
        path == "config.relay2_enabled" || path == "config.relay3_enabled" ||
        path == "config.relay4_enabled") {
      if (!v.is<bool>())
        return error_json(422, path + " must be true or false");
      yaml = curling_engine::yaml_bool(v.as<bool>());
    } else if (path == "config.pace_of_play.unit_duration") {
      if (!json_strict_int(v, &number) || number <= 0)
        return error_json(422, path + " must be a positive whole number of seconds");
      yaml = curling_engine::yaml_int(number);
    } else if (path == "config.header.font_size" || path == "config.countdown.font_size_4_digits" ||
               path == "config.countdown.font_size_5_digits" ||
               path == "config.pace_of_play.font_size" || path == "config.footer.font_size" ||
               path == "config.buzzer.duration" || path == "config.buzzer.header_hold" ||
               path == "config.buzzer.countdown.minutes_before" ||
               path == "config.relay_count" ||
               path == "config.pregame_countdown") {
      if (!json_strict_int(v, &number) || number < 0 || number > 100000)
        return error_json(422, path + " must be a whole number of seconds or pixels (0 or more)");
      yaml = curling_engine::yaml_int(number);
    } else if (path == "config.buzzer.relays" || path == "config.buzzer.ring_at_start" ||
               path == "config.buzzer.countdown.relays") {
      if (!v.is<JsonArrayConst>())
        return error_json(422, path + " must be a list of RELAY1, RELAY2, RELAY3 and/or RELAY4");
      std::vector<std::string> relays;
      for (JsonVariantConst relay : v.as<JsonArrayConst>()) {
        if (!relay.is<const char *>())
          return error_json(422, path + " must be a list of RELAY1, RELAY2, RELAY3 and/or RELAY4");
        const std::string name = relay.as<const char *>();
        bool known = false;
        for (const char *valid : VALID_RELAY_NAMES) {
          if (name == valid) {
            known = true;
            break;
          }
        }
        if (!known)
          return error_json(422, path + " must be a list of RELAY1, RELAY2, RELAY3 and/or RELAY4");
        relays.push_back(name);
      }
      yaml = curling_engine::yaml_relays(relays);
    } else if (path.rfind("config.header.text.", 0) == 0 ||
               path.rfind("config.header.color.", 0) == 0 ||
               path.rfind("config.countdown.color.", 0) == 0 ||
               path == "config.pace_of_play.color" || path == "config.pace_of_play.unit_name") {
      if (!v.is<const char *>())
        return error_json(422, path + " must be a single line of text");
      const std::string text = v.as<const char *>();
      if (text.find('\r') != std::string::npos || text.find('\n') != std::string::npos ||
          text.find('\t') != std::string::npos)
        return error_json(422, path + " must be a single line of text");
      yaml = curling_engine::yaml_quote(text);
    } else if (path == "config.relay1_function" || path == "config.relay2_function" ||
               path == "config.relay3_function" || path == "config.relay4_function") {
      if (!v.is<const char *>())
        return error_json(422, path + " must be a string");
      yaml = curling_engine::yaml_quote(v.as<const char *>());
    } else {
      return error_json(422, "unknown config setting '" + path + "'");
    }
    values.push_back(ConfigValue{path, yaml});
  }
  std::string new_version, err;
  const StoreStatus status = engine_->get_store()->update(
      [&values](const std::string &text, std::string *changed, std::string *cerr) {
        return curling_engine::apply_config_values(text, values, changed, cerr);
      },
      &version, &new_version, &err);
  if (status != StoreStatus::OK)
    return store_error(status, err);
  JsonDocument doc;
  doc["ok"] = true;
  doc["version"] = new_version;
  return from_document(doc);
}

// -- auth ---------------------------------------------------------------------

ApiResponse PiTimerApi::login(const ApiRequest &req, JsonObjectConst data) {
  int weekday = -1, sec = -1;
  std::string date_iso;
  if (!now_ok(&weekday, &sec, &date_iso))
    return error_json(503, "clock not synced yet");
  const float now = static_cast<float>(std::time(nullptr));
  if (limiter_->blocked(req.client_address, now))
    return error_json(429, "too many failed logins, wait a few minutes");
  JsonVariantConst password = data["password"];
  if (!password.is<const char *>() ||
      !check_password(password.as<const char *>(), config_->password_hash())) {
    limiter_->failed(req.client_address, now);
    return error_json(401, "wrong password");
  }
  ApiResponse resp = ok_json();
  resp.extra_headers.emplace_back("Set-Cookie",
                                  session_header(config_->make_session(now), SESSION_SECONDS));
  return resp;
}

ApiResponse PiTimerApi::logout() {
  ApiResponse resp = ok_json();
  resp.extra_headers.emplace_back("Set-Cookie", session_header("", 0));
  return resp;
}

// -- routing ------------------------------------------------------------------

ApiResponse PiTimerApi::handle(const ApiRequest &req) {
  const bool get = req.method == "GET";
  const bool post = req.method == "POST";
  if (post) {
    // python RequestHandler._read_json, same order
    if (!content_type_is_json(req.content_type))
      return error_json(415, "expected application/json");
    if (!origin_ok(req.origin, req.host))
      return error_json(403, "cross site request refused");
    if (req.bad_content_length)
      return error_json(400, "bad Content-Length");
    if (req.body_too_large)
      return error_json(413, "request too large");
  }
  const std::string &p = req.path;
  if (get) {
    if (p.rfind("/api/", 0) != 0)
      return error_json(404, "not found");
    if (!logged_in(req.cookie))
      return error_json(401, "please log in");
    if (p == "/api/status")
      return status();
    if (p == "/api/settings")
      return get_settings();
    if (p == "/api/raw")
      return get_raw();
    if (p == "/api/backups")
      return get_backups();
    if (p == "/api/config")
      return get_config();
    if (p.rfind("/api/backups/", 0) == 0)
      return get_backup(unquote_path(p.substr(13)));
    return error_json(404, "not found");
  }
  if (!post)
    return error_json(404, "not found");
  // python json.loads(rfile.read(length) or b'{}')
  const std::string raw = req.body.empty() ? std::string("{}") : req.body;
  JsonDocument body_doc;
  if (deserializeJson(body_doc, raw))
    return error_json(400, "request is not valid JSON");
  if (!body_doc.is<JsonObject>())
    return error_json(400, "request must be a JSON object");
  JsonObjectConst data = body_doc.as<JsonObjectConst>();
  if (p == "/api/login")
    return login(req, data);
  if (p == "/api/logout")
    return logout();
  if (!logged_in(req.cookie))
    return error_json(401, "please log in");
  if (p == "/api/settings")
    return put_settings(data);
  if (p == "/api/enabled")
    return set_enabled(data);
  if (p == "/api/raw")
    return put_raw(data);
  if (p == "/api/validate")
    return validate_raw(data);
  if (p == "/api/restore")
    return restore(data);
  if (p == "/api/quickstart")
    return quick_start(data);
  if (p == "/api/quickstart/cancel")
    return cancel_quick_start(data);
  if (p == "/api/test_relay")
    return test_relay(data);
  if (p == "/api/config")
    return post_config(data);
  return error_json(404, "not found");
}

}  // namespace web_editor
}  // namespace esphome
