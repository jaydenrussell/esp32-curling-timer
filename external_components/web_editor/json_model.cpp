#include "json_model.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace esphome {
namespace web_editor {

using curling_engine::DAY_NAMES;
using curling_engine::Entry;
using curling_engine::ExceptionDay;
using curling_engine::SECONDS_PER_DAY;
using curling_engine::ScheduleModel;

static constexpr const int DAYS_IN_MONTH[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// -- helpers (python _is_int / _time_value / _text_value / _date_value) ------

static bool json_is_int(JsonVariantConst v) {
  if (v.isNull() || v.is<bool>() || !v.is<long long>())
    return false;
  if (v.is<double>()) {
    const double d = v.as<double>();
    const double i = static_cast<double>(v.as<long long>());
    if (d != i)
      return false;  // fractional number — not an int in python
  }
  return true;
}

static bool time_value(JsonVariantConst v, const std::string &where, int *out, std::string *err) {
  if (!json_is_int(v)) {
    *err = where + " must be a time of day";
    return false;
  }
  const long long value = v.as<long long>();
  if (value < 0 || value >= SECONDS_PER_DAY) {
    *err = where + " must be a time of day";
    return false;
  }
  *out = static_cast<int>(value);
  return true;
}

static bool text_value(JsonVariantConst v, const std::string &where, std::string *out,
                       std::string *err) {
  if (v.isNull()) {
    out->clear();
    return true;
  }
  if (!v.is<const char *>()) {
    *err = where + " must be a single line of text";
    return false;
  }
  const char *s = v.as<const char *>();
  if (s[0] == '\0') {
    out->clear();
    return true;
  }
  if (std::strchr(s, '\r') != nullptr || std::strchr(s, '\n') != nullptr ||
      std::strchr(s, '\t') != nullptr) {
    *err = where + " must be a single line of text";
    return false;
  }
  *out = s;
  return true;
}

// python datetime.date.fromisoformat validation on an already \d{4}-\d{2}-\d{2} string.
static bool real_date(const std::string &value) {
  const int year = std::stoi(value.substr(0, 4));
  const int month = std::stoi(value.substr(5, 2));
  const int day = std::stoi(value.substr(8, 2));
  if (month < 1 || month > 12 || day < 1)
    return false;
  int last = DAYS_IN_MONTH[month - 1];
  if (month == 2 && (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0))
    last = 29;
  return day <= last;
}

// python repr() of a JSON scalar, for the `exception date {value!r}` messages.
static std::string repr_json_like(JsonVariantConst v) {
  if (v.isNull())
    return "None";
  if (v.is<bool>())
    return v.as<bool>() ? "True" : "False";
  if (json_is_int(v))
    return std::to_string(v.as<long long>());
  if (v.is<const char *>())
    return std::string("'") + v.as<const char *>() + "'";
  if (v.is<JsonArrayConst>() || v.is<JsonObjectConst>())
    return v.is<JsonArrayConst>() ? "[]" : "{}";
  char buf[64];
  std::snprintf(buf, sizeof(buf), "%g", v.as<double>());
  return buf;
}

// -- entry / model -----------------------------------------------------------

static void entry_into(JsonObject obj, const Entry &entry) {
  obj["start"] = entry.start;
  obj["buzzer"] = entry.buzzer;
  if (entry.name.has_value())
    obj["name"] = *entry.name;
  else
    obj["name"] = nullptr;
  if (entry.during_buzzer.has_value())
    obj["during_buzzer"] = *entry.during_buzzer;
  else
    obj["during_buzzer"] = nullptr;
  if (entry.header_hold.has_value())
    obj["header_hold"] = *entry.header_hold;
  else
    obj["header_hold"] = nullptr;
  if (entry.pace_of_play.has_value())
    obj["pace_of_play"] = *entry.pace_of_play;
  else
    obj["pace_of_play"] = nullptr;
  if (entry.pace_unit_duration.has_value())
    obj["pace_unit_duration"] = *entry.pace_unit_duration;
  else
    obj["pace_unit_duration"] = nullptr;
  obj["quick"] = entry.quick;
}

void entry_into_json(JsonObject obj, const Entry &entry) { entry_into(obj, entry); }

void add_entry_json(JsonArray array, const Entry &entry) { entry_into(array.add<JsonObject>(), entry); }

std::string entry_to_json(const Entry &entry) {
  JsonDocument doc;
  entry_into(doc.to<JsonObject>(), entry);
  std::string out;
  serializeJson(doc, out);
  return out;
}

static bool entry_from_json(JsonObjectConst data, const std::string &where, Entry *entry,
                            std::string *err) {
  if (data.isNull()) {
    *err = where + ": entry must be an object";
    return false;
  }
  if (!time_value(data["start"], where + " start", &entry->start, err))
    return false;
  if (!time_value(data["buzzer"], where + " buzzer", &entry->buzzer, err))
    return false;
  if (!text_value(data["name"], where + " name", &entry->name.emplace(), err))
    return false;
  if (entry->name->empty())
    entry->name.reset();
  if (!text_value(data["during_buzzer"], where + " during_buzzer", &entry->during_buzzer.emplace(),
                  err))
    return false;
  if (entry->during_buzzer->empty())
    entry->during_buzzer.reset();
  JsonVariantConst hold = data["header_hold"];
  if (!hold.isNull()) {
    if (!json_is_int(hold) || hold.as<long long>() < 0) {
      *err = where + ": header_hold must be a whole number of seconds (0 or more)";
      return false;
    }
    entry->header_hold = static_cast<int>(hold.as<long long>());
  }
  JsonVariantConst pace = data["pace_of_play"];
  if (!pace.isNull()) {
    if (!pace.is<bool>()) {
      *err = where + ": pace_of_play must be true, false or null";
      return false;
    }
    entry->pace_of_play = pace.as<bool>();
  }
  JsonVariantConst unit = data["pace_unit_duration"];
  if (!unit.isNull()) {
    if (!json_is_int(unit) || unit.as<long long>() <= 0) {
      *err = where + ": pace_unit_duration must be a positive number of seconds";
      return false;
    }
    entry->pace_unit_duration = static_cast<int>(unit.as<long long>());
  }
  entry->quick = data["quick"].as<bool>();
  return true;
}

std::string model_to_json(const ScheduleModel &model) {
  JsonDocument doc;
  JsonObject root = doc.to<JsonObject>();
  JsonObject schedule = root["schedule"].to<JsonObject>();
  for (const auto &kv : model.schedule) {
    JsonArray array = schedule[kv.first].to<JsonArray>();
    for (const Entry &entry : kv.second)
      add_entry_json(array, entry);
  }
  JsonArray exceptions = root["exceptions"].to<JsonArray>();
  for (const ExceptionDay &day : model.exceptions) {
    JsonObject obj = exceptions.add<JsonObject>();
    obj["date"] = day.date;
    obj["note"] = day.note;
    JsonArray array = obj["entries"].to<JsonArray>();
    for (const Entry &entry : day.entries)
      add_entry_json(array, entry);
  }
  root["inner_comments"] = model.inner_comments;
  std::string out;
  serializeJson(doc, out);
  return out;
}

bool model_from_json(JsonObjectConst data, ScheduleModel *out, std::string *err) {
  if (data.isNull()) {
    *err = "settings must be an object";
    return false;
  }
  JsonVariantConst raw_schedule = data["schedule"];
  JsonVariantConst raw_exceptions = data["exceptions"];
  if (!raw_schedule.is<JsonObjectConst>() || !raw_exceptions.is<JsonArrayConst>()) {
    *err = "expected 'schedule' object and 'exceptions' list";
    return false;
  }
  JsonObjectConst schedule_obj = raw_schedule.as<JsonObjectConst>();
  ScheduleModel model;
  for (const char *day : DAY_NAMES) {
    JsonVariantConst raw_entries = schedule_obj[day];
    if (!raw_entries.isNull() && !raw_entries.is<JsonArrayConst>()) {
      *err = std::string(day) + ": entries must be a list";
      return false;
    }
    std::vector<Entry> entries;
    if (!raw_entries.isNull()) {
      size_t i = 0;
      for (JsonVariantConst raw_entry : raw_entries.as<JsonArrayConst>()) {
        std::string where = std::string(day) + " entry " + std::to_string(i + 1);
        entries.emplace_back();
        if (!entry_from_json(raw_entry.as<JsonObjectConst>(), where, &entries.back(), err))
          return false;
        i++;
      }
    }
    model.schedule[day] = std::move(entries);
  }
  std::vector<std::string> unknown;
  for (JsonPairConst kv : schedule_obj) {
    bool known = false;
    for (const char *day : DAY_NAMES) {
      if (std::strcmp(kv.key().c_str(), day) == 0) {
        known = true;
        break;
      }
    }
    if (!known)
      unknown.emplace_back(kv.key().c_str());
  }
  if (!unknown.empty()) {
    std::sort(unknown.begin(), unknown.end());
    std::string joined;
    for (const std::string &name : unknown) {
      if (!joined.empty())
        joined += ", ";
      joined += name;
    }
    *err = "unknown schedule day(s): " + joined;
    return false;
  }
  for (JsonVariantConst raw_day : raw_exceptions.as<JsonArrayConst>()) {
    JsonObjectConst day_obj = raw_day.as<JsonObjectConst>();
    if (day_obj.isNull()) {
      *err = "each exception must be an object";
      return false;
    }
    JsonVariantConst raw_date = day_obj["date"];
    std::string date;
    if (!raw_date.is<const char *>()) {
      *err = "exception date " + repr_json_like(raw_date) + " must be YYYY-MM-DD";
      return false;
    }
    date = raw_date.as<const char *>();
    bool shape_ok = date.size() == 10 && date[4] == '-' && date[7] == '-';
    for (size_t i = 0; shape_ok && i < date.size(); i++) {
      if (i == 4 || i == 7)
        continue;
      if (date[i] < '0' || date[i] > '9')
        shape_ok = false;
    }
    if (!shape_ok) {
      *err = "exception date '" + date + "' must be YYYY-MM-DD";
      return false;
    }
    if (!real_date(date)) {
      *err = "exception date " + date + " is not a real date";
      return false;
    }
    for (const ExceptionDay &seen : model.exceptions) {
      if (seen.date == date) {
        *err = "exception date " + date + " is listed twice";
        return false;
      }
    }
    ExceptionDay day;
    day.date = date;
    if (!text_value(day_obj["note"], date + " note", &day.note, err))
      return false;
    JsonVariantConst raw_entries = day_obj["entries"];
    if (!raw_entries.isNull() && !raw_entries.is<JsonArrayConst>()) {
      *err = date + ": entries must be a list";
      return false;
    }
    if (!raw_entries.isNull()) {
      size_t i = 0;
      for (JsonVariantConst raw_entry : raw_entries.as<JsonArrayConst>()) {
        std::string where = date + " entry " + std::to_string(i + 1);
        day.entries.emplace_back();
        if (!entry_from_json(raw_entry.as<JsonObjectConst>(), where, &day.entries.back(), err))
          return false;
        i++;
      }
    }
    model.exceptions.push_back(std::move(day));
  }
  *out = std::move(model);
  return true;
}

}  // namespace web_editor
}  // namespace esphome
