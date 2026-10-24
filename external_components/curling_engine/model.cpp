#include "model.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <set>
#include <tuple>

namespace esphome {
namespace curling_engine {

namespace {

constexpr const char *KEY_INDENT = "    ";
constexpr const char *ENTRY_INDENT = "        ";

// -------------------------------------------------- text utilities --------

bool is_blank(const std::string &s) {
  return s.find_first_not_of(" \t") == std::string::npos;
}

std::string strip(const std::string &s) {
  size_t a = s.find_first_not_of(" \t");
  if (a == std::string::npos)
    return "";
  size_t b = s.find_last_not_of(" \t");
  return s.substr(a, b - a + 1);
}

// -------------------------------------------------- regex ports -----------

// ^[A-Za-z_]\w*\s*:(.*)$ — a top level (column 0) mapping key
bool top_key_match(const std::string &line, std::string *key, std::string *rest) {
  if (line.empty())
    return false;
  char c0 = line[0];
  if (!(std::isalpha(static_cast<unsigned char>(c0)) || c0 == '_'))
    return false;
  size_t i = 1;
  while (i < line.size() &&
         (std::isalnum(static_cast<unsigned char>(line[i])) || line[i] == '_'))
    i++;
  size_t j = i;
  while (j < line.size() && (line[j] == ' ' || line[j] == '\t'))
    j++;
  if (j >= line.size() || line[j] != ':')
    return false;
  if (key != nullptr)
    *key = line.substr(0, i);
  if (rest != nullptr)
    *rest = line.substr(j + 1);
  return true;
}

// ^\s+(\d{4}-\d{2}-\d{2})\s*:(.*)$ — an indented exception date key
bool date_key_match(const std::string &line, std::string *date, std::string *rest) {
  size_t i = 0;
  size_t ws = 0;
  while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
    i++;
    ws++;
  }
  if (ws == 0)
    return false;
  if (i + 10 > line.size())
    return false;
  for (size_t k = 0; k < 4; k++)
    if (!std::isdigit(static_cast<unsigned char>(line[i + k])))
      return false;
  if (line[i + 4] != '-')
    return false;
  for (size_t k = 5; k < 7; k++)
    if (!std::isdigit(static_cast<unsigned char>(line[i + k])))
      return false;
  if (line[i + 7] != '-')
    return false;
  for (size_t k = 8; k < 10; k++)
    if (!std::isdigit(static_cast<unsigned char>(line[i + k])))
      return false;
  size_t j = i + 10;
  while (j < line.size() && (line[j] == ' ' || line[j] == '\t'))
    j++;
  if (j >= line.size() || line[j] != ':')
    return false;
  if (date != nullptr)
    *date = line.substr(i, 10);
  if (rest != nullptr)
    *rest = line.substr(j + 1);
  return true;
}

// ^\s+-\s*\{.*\}\s*(#.*)?$ — a one-line flow entry; greedy '}' like the regex,
// comment (when present) returned including its '#'
bool entry_line_match(const std::string &line, std::string *comment) {
  size_t i = 0;
  while (i < line.size() && (line[i] == ' ' || line[i] == '\t'))
    i++;
  if (i >= line.size() || line[i] != '-')
    return false;
  i++;
  while (i < line.size() && (line[i] == ' ' || line[i] == '\t'))
    i++;
  if (i >= line.size() || line[i] != '{')
    return false;
  size_t close = line.rfind('}');
  if (close == std::string::npos || close < i)
    return false;
  size_t j = close + 1;
  while (j < line.size() && (line[j] == ' ' || line[j] == '\t'))
    j++;
  if (j < line.size() && line[j] != '#')
    return false;
  if (comment != nullptr)
    *comment = j < line.size() ? line.substr(j) : std::string();
  return true;
}

// Python str() of a parsed YAML scalar (name fields may arrive as numbers or
// bools; the settings file wants them as text).
std::string python_str(const YamlValue &v) {
  switch (v.type) {
    case YType::Str:
      return v.s;
    case YType::Bool:
      return v.b ? "True" : "False";
    case YType::Int: {
      char buf[32];
      snprintf(buf, sizeof(buf), "%lld", v.i);
      return buf;
    }
    case YType::Float:
      return yaml_repr(v);
    case YType::Null:
      return "None";
    case YType::Seq:
      return "[...]";
    case YType::Map:
      return "{...}";
  }
  return "None";
}

// Python truthiness of a parsed value (for `raw.get(...) or {}`).
bool yaml_falsy(const YamlValue &v) {
  switch (v.type) {
    case YType::Null:
      return true;
    case YType::Bool:
      return !v.b;
    case YType::Int:
      return v.i == 0;
    case YType::Float:
      return v.d == 0.0;
    case YType::Str:
      return v.s.empty();
    case YType::Seq:
      return v.seq.empty();
    case YType::Map:
      return v.map.empty();
  }
  return false;
}

// YYYY-MM-DD digits only
bool date_shape_ok(const std::string &s) {
  if (s.size() != 10)
    return false;
  for (size_t k = 0; k < 10; k++) {
    if (k == 4 || k == 7) {
      if (s[k] != '-')
        return false;
    } else if (!std::isdigit(static_cast<unsigned char>(s[k]))) {
      return false;
    }
  }
  return true;
}

bool is_real_date(int y, int m, int d) {
  static const int kDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (m < 1 || m > 12 || d < 1)
    return false;
  int maxd = kDays[m];
  if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
    maxd = 29;
  return d <= maxd;
}

// -------------------------------------------------- entries ----------------

// editor.py Entry.from_json — also the validator for YAML-parsed entries.
bool entry_from_map(const YamlValue &data, const std::string &where, Entry *out,
                    std::string *err) {
  if (data.type != YType::Map) {
    *err = where + ": entry must be an object";
    return false;
  }
  auto time_value = [&](const char *key) -> int {
    const YamlValue *v = data.find(key);
    if (v != nullptr && v->type == YType::Int && v->i >= 0 && v->i < SECONDS_PER_DAY)
      return static_cast<int>(v->i);
    *err = where + " " + key + " must be a time of day";
    return -1;
  };
  out->start = time_value("start");
  if (out->start < 0)
    return false;
  out->buzzer = time_value("buzzer");
  if (out->buzzer < 0)
    return false;

  const YamlValue *v = data.find("name");
  out->name = std::nullopt;
  if (v != nullptr && v->type != YType::Null) {
    if (v->type != YType::Str || v->s.find_first_of("\r\n\t") != std::string::npos) {
      *err = where + " name must be a single line of text";
      return false;
    }
    if (!v->s.empty())
      out->name = v->s;
  }
  v = data.find("during_buzzer");
  out->during_buzzer = std::nullopt;
  if (v != nullptr && v->type != YType::Null) {
    if (v->type != YType::Str || v->s.find_first_of("\r\n\t") != std::string::npos) {
      *err = where + " during_buzzer must be a single line of text";
      return false;
    }
    if (!v->s.empty())
      out->during_buzzer = v->s;
  }
  v = data.find("header_hold");
  out->header_hold = std::nullopt;
  if (v != nullptr && v->type != YType::Null) {
    if (v->type != YType::Int || v->i < 0) {
      *err = where + ": header_hold must be a whole number of seconds (0 or more)";
      return false;
    }
    out->header_hold = static_cast<int>(v->i);
  }
  v = data.find("pace_of_play");
  out->pace_of_play = std::nullopt;
  if (v != nullptr && v->type != YType::Null) {
    if (v->type != YType::Bool) {
      *err = where + ": pace_of_play must be true, false or null";
      return false;
    }
    out->pace_of_play = v->b;
  }
  v = data.find("pace_unit_duration");
  out->pace_unit_duration = std::nullopt;
  if (v != nullptr && v->type != YType::Null) {
    if (v->type != YType::Int || v->i <= 0) {
      *err = where + ": pace_unit_duration must be a positive number of seconds";
      return false;
    }
    out->pace_unit_duration = static_cast<int>(v->i);
  }
  v = data.find("quick");
  out->quick = v != nullptr && v->type == YType::Bool && v->b;
  return true;
}

// editor.py _entries_from_yaml
bool entries_from_yaml(const YamlValue *raw, const std::string &where, std::vector<Entry> *out,
                       std::string *err) {
  out->clear();
  if (raw == nullptr || raw->type == YType::Null)
    return true;
  if (raw->type != YType::Seq) {
    *err = where + " must be a list of entries";
    return false;
  }
  for (size_t index = 0; index < raw->seq.size(); index++) {
    std::string entry_where = where + " entry " + std::to_string(index + 1);
    const YamlValue *item = &raw->seq[index];
    if (item->type != YType::Map) {
      *err = entry_where + " is not a {start: ..., buzzer: ...} entry";
      return false;
    }
    static const char *kKnown[] = {"start",       "buzzer",            "name",
                                   "during_buzzer", "header_hold",      "pace_of_play",
                                   "pace_unit_duration"};
    std::set<std::string> unknown;
    for (const auto &kv : item->map) {
      bool known = false;
      for (const char *k : kKnown)
        if (kv.first == k)
          known = true;
      if (!known)
        unknown.insert(kv.first);
    }
    if (!unknown.empty()) {
      std::string joined;
      for (const std::string &u : unknown) {
        if (!joined.empty())
          joined += ", ";
        joined += u;
      }
      *err = entry_where + " has unknown key(s) " + joined;
      return false;
    }
    // name is optional but YAML may have turned it into a number or bool
    YamlValue copy = *item;
    const YamlValue *nv = copy.find("name");
    if (nv != nullptr && nv->type != YType::Null) {
      YamlValue conv;
      conv.type = YType::Str;
      conv.s = python_str(*nv);
      for (auto &kv : copy.map)
        if (kv.first == "name")
          kv.second = conv;
    }
    Entry e;
    if (!entry_from_map(copy, entry_where, &e, err))
      return false;
    out->push_back(std::move(e));
  }
  return true;
}

// -------------------------------------------------- section spans ----------

std::string comment_text(const std::string &rest) {
  size_t index = rest.find('#');
  if (index == std::string::npos)
    return "";
  return strip(rest.substr(index + 1));
}

// -------------------------------------------------- scanning ---------------

// editor.py _scan_exceptions: per date notes, per date entry quick start
// flags, and the inner comment count.
struct ExceptionScan {
  std::map<std::string, std::string> notes;
  std::map<std::string, std::vector<bool>> quick_flags;
  int inner_comments{0};
};

bool scan_exceptions(const std::vector<std::string> &lines, ExceptionScan *out, std::string *err) {
  out->notes.clear();
  out->quick_flags.clear();
  out->inner_comments = 0;
  size_t start, end;
  if (!section_span(lines, "exceptions", &start, &end, err))
    return false;
  std::string current;
  bool have_current = false;
  for (size_t i = start + 1; i < end; i++) {
    const std::string &line = lines[i];
    std::string date, rest;
    if (date_key_match(line, &date, &rest)) {
      current = date;
      have_current = true;
      out->notes[current] = comment_text(rest);
      out->quick_flags[current] = {};
    } else {
      std::string comment;
      if (entry_line_match(line, &comment) && have_current) {
        out->quick_flags[current].push_back(strip(comment) == QUICK_COMMENT);
      } else if (strip(line).rfind("#", 0) == 0) {
        out->inner_comments++;
      }
    }
  }
  return true;
}

// editor.py _count_inner_comments
bool count_inner_comments(const std::vector<std::string> &lines, const std::string &key,
                          int *out, std::string *err) {
  size_t start, end;
  if (!section_span(lines, key, &start, &end, err))
    return false;
  int n = 0;
  for (size_t i = start + 1; i < end; i++)
    if (strip(lines[i]).rfind("#", 0) == 0)
      n++;
  *out = n;
  return true;
}

// editor.py _exceptions_from_yaml
bool exceptions_from_yaml(const YamlValue &raw_exceptions, const std::string &text,
                          std::vector<ExceptionDay> *out, int *inner_comments, std::string *err) {
  if (raw_exceptions.type != YType::Map) {
    *err = "'exceptions' section must be a mapping of dates";
    return false;
  }
  std::vector<std::string> lines;
  std::string newline;
  lines = split_lines(text, &newline);
  ExceptionScan scan;
  if (!scan_exceptions(lines, &scan, err))
    return false;
  out->clear();
  for (const auto &kv : raw_exceptions.map) {
    std::string date = kv.first;
    if (!date_shape_ok(date)) {
      *err = "exception date '" + date + "' must be YYYY-MM-DD";
      return false;
    }
    int y = 0, m = 0, d = 0;
    sscanf(date.c_str(), "%4d-%2d-%2d", &y, &m, &d);
    if (!is_real_date(y, m, d)) {
      *err = "exception date " + date + " is not a real date";
      return false;
    }
    std::vector<Entry> entries;
    if (!entries_from_yaml(&kv.second, date, &entries, err))
      return false;
    auto fit = scan.quick_flags.find(date);
    const std::vector<bool> *flags = fit != scan.quick_flags.end() ? &fit->second : nullptr;
    // only trust the per line markers when they line up with the parsed entries
    if (flags != nullptr && flags->size() == entries.size()) {
      for (size_t i = 0; i < entries.size(); i++)
        entries[i].quick = (*flags)[i];
    }
    auto nit = scan.notes.find(date);
    out->push_back(ExceptionDay{date, nit != scan.notes.end() ? nit->second : "", entries});
  }
  *inner_comments = scan.inner_comments;
  return true;
}

// -------------------------------------------------- rendering --------------

std::string quote(const std::string &text) {
  std::string out = "'";
  for (char c : text) {
    if (c == '\'')
      out += "''";
    else
      out.push_back(c);
  }
  out.push_back('\'');
  return out;
}

// always written in start time order, however the entries arrived
std::vector<std::string> render_entries(const std::vector<Entry> &entries) {
  std::vector<Entry> sorted = entries;
  std::stable_sort(sorted.begin(), sorted.end(),
                   [](const Entry &a, const Entry &b) { return a.start < b.start; });
  std::vector<std::string> lines;
  lines.reserve(sorted.size());
  for (const Entry &e : sorted)
    lines.push_back(std::string(ENTRY_INDENT) + render_entry(e));
  return lines;
}

std::vector<std::string> schedule_lines(const ScheduleModel &model) {
  std::vector<std::string> lines;
  for (const char *day : DAY_NAMES) {
    auto it = model.schedule.find(day);
    const std::vector<Entry> *entries = it != model.schedule.end() ? &it->second : nullptr;
    bool empty = entries == nullptr || entries->empty();
    lines.push_back(std::string(KEY_INDENT) + day + ":" + (empty ? " []" : ""));
    if (entries != nullptr) {
      for (const std::string &l : render_entries(*entries))
        lines.push_back(l);
    }
  }
  return lines;
}

// editor.py _exceptions_lines: regenerated exceptions section, key line included
std::vector<std::string> exceptions_lines(const std::string &key_line_in,
                                          const std::vector<std::string> &old_lines,
                                          const ScheduleModel &model) {
  if (model.exceptions.empty()) {
    // the validator needs a mapping, so an empty section is '{}' (not '[]')
    return {std::string("exceptions: {}")};
  }
  // an inline empty value like 'exceptions: {}' has to go once there are dates
  std::string key_line = key_line_in;
  {
    std::string rest;
    top_key_match(key_line, nullptr, &rest);
    size_t hash = rest.find('#');
    if (hash != std::string::npos)
      rest = rest.substr(0, hash);
    if (!strip(rest).empty())
      key_line = "exceptions:";
  }
  // original date lines, reused when their note and empty/non-empty state are
  // unchanged
  std::map<std::string, std::tuple<std::string, std::string, bool>> kept;
  for (const std::string &line : old_lines) {
    std::string date, rest;
    if (date_key_match(line, &date, &rest)) {
      std::string before = rest;
      size_t hash = before.find('#');
      if (hash != std::string::npos)
        before = before.substr(0, hash);
      kept[date] = std::make_tuple(line, comment_text(rest), before.find("[]") != std::string::npos);
    }
  }
  std::vector<std::string> result;
  result.push_back(key_line);
  for (const ExceptionDay &day : model.exceptions) {
    auto old = kept.find(day.date);
    if (old != kept.end() &&
        std::get<1>(old->second) == day.note &&
        std::get<2>(old->second) == day.entries.empty()) {
      result.push_back(std::get<0>(old->second));
    } else {
      std::string note = day.note.empty() ? "" : " # " + day.note;
      result.push_back(std::string(KEY_INDENT) + day.date + ":" +
                       (day.entries.empty() ? " []" : "") + note);
    }
    for (const std::string &l : render_entries(day.entries))
      result.push_back(l);
  }
  return result;
}

}  // namespace

// -------------------------------------------------- public API ------------

bool section_span(const std::vector<std::string> &lines, const std::string &key, size_t *start,
                  size_t *end, std::string *err) {
  size_t s = std::string::npos;
  for (size_t i = 0; i < lines.size(); i++) {
    std::string k;
    if (top_key_match(lines[i], &k, nullptr) && k == key) {
      s = i;
      break;
    }
  }
  if (s == std::string::npos) {
    if (err != nullptr)
      *err = "settings file has no top level '" + key + ":' section";
    return false;
  }
  size_t e = lines.size();
  for (size_t i = s + 1; i < lines.size(); i++) {
    if (top_key_match(lines[i], nullptr, nullptr)) {
      e = i;
      break;
    }
  }
  while (e > s + 1 && (is_blank(lines[e - 1]) || lines[e - 1][0] == '#'))
    e--;
  *start = s;
  *end = e;
  return true;
}

std::string render_entry(const Entry &entry) {
  std::string line = "- {start: " + format_time(entry.start) + ", buzzer: " +
                     format_time(entry.buzzer);
  if (entry.name.has_value())
    line += ", name: " + quote(*entry.name);
  if (entry.during_buzzer.has_value())
    line += ", during_buzzer: " + quote(*entry.during_buzzer);
  if (entry.header_hold.has_value())
    line += ", header_hold: " + std::to_string(*entry.header_hold);
  if (entry.pace_of_play.has_value())
    line += std::string(", pace_of_play: ") + (*entry.pace_of_play ? "True" : "False");
  if (entry.pace_unit_duration.has_value())
    line += ", pace_unit_duration: " + std::to_string(*entry.pace_unit_duration);
  line += "}";
  if (entry.quick)
    line += std::string("  ") + QUICK_COMMENT;
  return line;
}

std::vector<std::string> split_lines(const std::string &text, std::string *newline) {
  *newline = text.find("\r\n") != std::string::npos ? "\r\n" : "\n";
  std::string clean = text;
  size_t at = 0;
  while ((at = clean.find("\r\n", at)) != std::string::npos) {
    clean.replace(at, 2, "\n");
    at++;
  }
  std::vector<std::string> lines;
  size_t begin = 0;
  while (true) {
    size_t nl = clean.find('\n', begin);
    if (nl == std::string::npos) {
      lines.push_back(clean.substr(begin));
      break;
    }
    lines.push_back(clean.substr(begin, nl - begin));
    begin = nl + 1;
  }
  return lines;
}

bool load_model(const std::string &text, ScheduleModel *out, std::string *err) {
  YamlValue raw;
  if (!yaml_parse(text, &raw, err)) {
    *err = "YAML error: " + *err;
    return false;
  }
  if (raw.type != YType::Map) {
    *err = "settings file is empty or not a YAML mapping";
    return false;
  }
  const YamlValue *raw_schedule = raw.find("schedule");
  if (raw_schedule == nullptr || raw_schedule->type != YType::Map) {
    *err = "'schedule' section is missing or not a mapping";
    return false;
  }
  ScheduleModel model;
  for (const char *day : DAY_NAMES) {
    const YamlValue *entries = raw_schedule->find(day);
    if (!entries_from_yaml(entries, day, &model.schedule[day], err))
      return false;
  }
  std::set<std::string> unknown;
  for (const auto &kv : raw_schedule->map) {
    bool known = false;
    for (const char *day : DAY_NAMES)
      if (kv.first == day)
        known = true;
    if (!known)
      unknown.insert(kv.first);
  }
  if (!unknown.empty()) {
    std::string joined;
    for (const std::string &u : unknown) {
      if (!joined.empty())
        joined += ", ";
      joined += u;
    }
    *err = "unknown schedule day(s): " + joined;
    return false;
  }

  std::vector<std::string> lines;
  std::string newline;
  lines = split_lines(text, &newline);

  YamlValue empty_map;
  empty_map.type = YType::Map;
  const YamlValue *exc_v = raw.find("exceptions");
  const YamlValue *use_exc = &empty_map;
  if (exc_v != nullptr && !yaml_falsy(*exc_v))
    use_exc = exc_v;  // truthy; exceptions_from_yaml rejects non-mappings

  int exceptions_inner = 0;
  if (!exceptions_from_yaml(*use_exc, text, &model.exceptions, &exceptions_inner, err))
    return false;
  int schedule_inner = 0;
  if (!count_inner_comments(lines, "schedule", &schedule_inner, err))
    return false;
  model.inner_comments = exceptions_inner + schedule_inner;
  *out = std::move(model);
  return true;
}

bool splice(const std::string &text, const ScheduleModel &model, std::string *out,
            std::string *err) {
  std::string newline;
  std::vector<std::string> lines = split_lines(text, &newline);
  size_t start, end;
  if (!section_span(lines, "exceptions", &start, &end, err))
    return false;
  std::vector<std::string> old_body(lines.begin() + static_cast<long>(start) + 1,
                                    lines.begin() + static_cast<long>(end));
  std::vector<std::string> repl = exceptions_lines(lines[start], old_body, model);
  lines.erase(lines.begin() + static_cast<long>(start),
              lines.begin() + static_cast<long>(end));
  lines.insert(lines.begin() + static_cast<long>(start), repl.begin(), repl.end());
  if (!section_span(lines, "schedule", &start, &end, err))
    return false;
  std::vector<std::string> body = schedule_lines(model);
  lines.erase(lines.begin() + static_cast<long>(start) + 1,
              lines.begin() + static_cast<long>(end));
  lines.insert(lines.begin() + static_cast<long>(start) + 1, body.begin(), body.end());
  out->clear();
  for (size_t i = 0; i < lines.size(); i++) {
    if (i > 0)
      *out += newline;
    *out += lines[i];
  }
  return true;
}

bool set_enabled(const std::string &text, bool enabled, std::string *out, std::string *err) {
  std::string newline;
  std::vector<std::string> lines = split_lines(text, &newline);
  size_t start, end;
  if (!section_span(lines, "config", &start, &end, err))
    return false;
  bool indent_set = false;
  std::string indent;
  for (size_t index = start + 1; index < end; index++) {
    const std::string &line = lines[index];
    if (is_blank(line) || strip(line).rfind("#", 0) == 0)
      continue;
    size_t nz = line.find_first_not_of(" \t");
    std::string line_indent = line.substr(0, nz);
    if (!indent_set || indent.empty()) {
      indent = line_indent;
      indent_set = true;
    }
    if (line_indent != indent)
      continue;  // deeper line, e.g. pace_of_play.enabled
    // ^(\s+enabled\s*:\s*)(\S+)(.*)$
    size_t p = 0;
    size_t ws = 0;
    while (p < line.size() && (line[p] == ' ' || line[p] == '\t')) {
      p++;
      ws++;
    }
    if (ws == 0)
      continue;
    if (line.compare(p, 7, "enabled") != 0)
      continue;
    p += 7;
    while (p < line.size() && (line[p] == ' ' || line[p] == '\t'))
      p++;
    if (p >= line.size() || line[p] != ':')
      continue;
    p++;
    while (p < line.size() && (line[p] == ' ' || line[p] == '\t'))
      p++;
    size_t vstart = p;
    while (p < line.size() && line[p] != ' ' && line[p] != '\t')
      p++;
    if (p == vstart)
      continue;  // \S+ needs at least one character
    lines[index] = line.substr(0, vstart) + (enabled ? "True" : "False") + line.substr(p);
    out->clear();
    for (size_t i = 0; i < lines.size(); i++) {
      if (i > 0)
        *out += newline;
      *out += lines[i];
    }
    return true;
  }
  *err = "no 'enabled:' setting found in the config section";
  return false;
}

std::string format_time(int seconds) {
  seconds = seconds % (24 * 60 * 60);
  if (seconds < 0) seconds += (24 * 60 * 60);
  int hours = seconds / 3600;
  int rest = seconds % 3600;
  int minutes = rest / 60;
  int secs = rest % 60;
  if (hours == 0)
    return std::to_string(seconds);
  char buf[16];
  snprintf(buf, sizeof(buf), "%2d:%02d:%02d", hours, minutes, secs);
  return buf;
}

int weekday_mon0_from_iso(const std::string &date_iso) {
  if (!date_shape_ok(date_iso))
    return -1;
  int y = 0, m = 0, d = 0;
  sscanf(date_iso.c_str(), "%4d-%2d-%2d", &y, &m, &d);
  if (!is_real_date(y, m, d))
    return -1;
  // days from civil (Howard Hinnant); 1970-01-01 was a Thursday (= 3 Monday-based)
  y -= m <= 2;
  const long long era = (y >= 0 ? y : y - 399) / 400;
  const unsigned yoe = static_cast<unsigned>(y - era * 400);
  const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  long long days = era * 146097LL + static_cast<long long>(doe) - 719468;
  int wd = static_cast<int>((days % 7 + 7 + 3) % 7);
  return wd;
}

int entry_hold(const Entry &entry, int default_hold) {
  return entry.header_hold.has_value() ? *entry.header_hold : default_hold;
}

bool entries_overlap(const Entry &first, const Entry &second, int default_hold) {
  return first.start <= second.buzzer + entry_hold(second, default_hold) &&
         second.start <= first.buzzer + entry_hold(first, default_hold);
}

const ExceptionDay *ScheduleModel::exception_for(const std::string &date) const {
  for (const ExceptionDay &day : exceptions)
    if (day.date == date)
      return &day;
  return nullptr;
}

ExceptionDay *ScheduleModel::exception_for(const std::string &date) {
  for (ExceptionDay &day : exceptions)
    if (day.date == date)
      return &day;
  return nullptr;
}

std::vector<Entry> ScheduleModel::entries_for(const std::string &date_iso) const {
  const ExceptionDay *ex = exception_for(date_iso);
  if (ex != nullptr)
    return ex->entries;
  int wd = weekday_mon0_from_iso(date_iso);
  if (wd < 0)
    return {};
  auto it = schedule.find(DAY_NAMES[wd]);
  if (it == schedule.end())
    return {};
  return it->second;
}

}  // namespace curling_engine
}  // namespace esphome
