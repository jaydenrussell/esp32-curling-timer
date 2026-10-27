#include "config_edit.h"

#include "model.h"
#include "settings.h"

namespace esphome {
namespace curling_engine {

std::string yaml_bool(bool value) { return value ? "True" : "False"; }

std::string yaml_int(long long value) { return std::to_string(value); }

std::string yaml_quote(const std::string &value) {
  std::string out = "'";
  for (char c : value) {
    if (c == '\'')
      out += "''";
    else
      out.push_back(c);
  }
  out.push_back('\'');
  return out;
}

std::string yaml_relays(const std::vector<std::string> &relays) {
  std::string out = "[";
  for (size_t i = 0; i < relays.size(); i++) {
    if (i != 0)
      out += ", ";
    out += yaml_quote(relays[i]);
  }
  out += "]";
  return out;
}

static bool line_is_blank_or_comment(const std::string &line) {
  for (char c : line) {
    if (c == ' ' || c == '\t')
      continue;
    return c == '#';
  }
  return true;  // blank
}

static size_t line_indent(const std::string &line) {
  size_t i = 0;
  while (i < line.size() && (line[i] == ' ' || line[i] == '\t'))
    i++;
  return i;
}

// True when line at column `indent` opens `key:` (a mapping key line).
static bool line_opens_key(const std::string &line, size_t indent, const std::string &key) {
  if (line.size() <= indent || line[indent] == ' ' || line[indent] == '\t' || line[indent] == '#')
    return false;
  size_t i = indent;
  while (i < line.size()) {
    const char c = line[i];
    const bool word = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                      c == '_';
    if (!word)
      break;
    i++;
  }
  if (i == indent || i >= line.size() || line[i] != ':')
    return false;
  return line.substr(indent, i - indent) == key;
}

// Index of the first `key:` line that is a direct child of the key line at
// parent (indent strictly deeper, first hit = shallowest = direct child).
// parent == -1 means a top level key (found anywhere, like python's
// _section_span).
static bool find_child_line(const std::vector<std::string> &lines, long parent, const std::string &key,
                            size_t *index) {
  const size_t parent_indent = parent < 0 ? 0 : line_indent(lines[parent]);
  const size_t min_indent = parent < 0 ? 0 : parent_indent + 1;
  const size_t start = parent < 0 ? 0 : static_cast<size_t>(parent) + 1;
  for (size_t i = start; i < lines.size(); i++) {
    const std::string &line = lines[i];
    if (line_is_blank_or_comment(line))
      continue;
    const size_t indent = line_indent(line);
    if (indent < min_indent)
      return false;  // block ended
    if (line_opens_key(line, indent, key)) {
      *index = i;
      return true;
    }
  }
  return false;
}

// Replace the scalar after `key:` on one line, keeping any trailing comment.
// A block value (empty scalar + following deeper lines, or same-indent sequence
// items like pyyaml's `key:\n- item`) is removed so the scalar replaces it.
static bool replace_line_value(std::vector<std::string> *lines, size_t index, const std::string &key,
                               const std::string &yaml) {
  std::string &line = (*lines)[index];
  const size_t indent = line_indent(line);
  if (!line_opens_key(line, indent, key))
    return false;
  const size_t colon = indent + key.size();
  size_t value_start = colon + 1;
  while (value_start < line.size() && (line[value_start] == ' ' || line[value_start] == '\t'))
    value_start++;
  const bool empty_value =
      value_start >= line.size() || line[value_start] == '#';
  if (!empty_value) {
    size_t value_end = value_start;
    if (line[value_start] == '\'') {
      // quoted scalar: skip over '' escapes to the closing quote
      value_end = value_start + 1;
      while (value_end < line.size()) {
        if (line[value_end] == '\'') {
          if (value_end + 1 < line.size() && line[value_end + 1] == '\'') {
            value_end += 2;
            continue;
          }
          value_end++;  // include the closing quote
          break;
        }
        value_end++;
      }
      if (value_end > line.size() || (value_end == line.size() && line[value_end - 1] != '\''))
        return false;  // unterminated quote — leave the file alone
    } else {
      while (value_end < line.size() && line[value_end] != ' ' && line[value_end] != '\t' &&
             line[value_end] != '#')
        value_end++;
    }
    // python set_enabled keeps the rest of the line (spacing + comment) verbatim
    const std::string tail = line.substr(value_end);
    line = line.substr(0, value_start) + yaml + tail;
    return true;
  }

  // empty scalar: drop the block body that follows, then put the scalar inline
  size_t drop_end = index + 1;
  while (drop_end < lines->size()) {
    const std::string &next = (*lines)[drop_end];
    if (line_is_blank_or_comment(next)) {
      // blanks/comments inside the block go with it; a column-0 comment before
      // the next top level key does not (stop on non-blank comment later)
      drop_end++;
      continue;
    }
    const size_t next_indent = line_indent(next);
    const bool same_indent_seq = next_indent == indent && next.size() > indent && next[indent] == '-';
    if (next_indent > indent || same_indent_seq) {
      drop_end++;
      continue;
    }
    break;
  }
  lines->erase(lines->begin() + static_cast<long>(index) + 1,
               lines->begin() + static_cast<long>(drop_end));
  line = line.substr(0, colon + 1) + " " + yaml;
  return true;
}

// Append `key: <yaml>` as the last line of the block opened at parent (used
// when a newer setting is missing from an older file). An empty `yaml`
// creates just the parent key line for nested paths. Returns the inserted
// line index. Child indent matches the first existing child line, else
// parent + 2.
static size_t insert_child_value(std::vector<std::string> *lines, size_t parent,
                                 const std::string &key, const std::string &yaml) {
  const size_t parent_indent = line_indent((*lines)[parent]);
  size_t child_indent = parent_indent + 2;
  bool have_child_indent = false;
  // Empty block (just the parent key line): insert immediately after it.
  size_t insert_at = parent + 1;
  for (size_t i = parent + 1; i < lines->size(); i++) {
    const std::string &line = (*lines)[i];
    const size_t indent = line_indent(line);
    if (line_is_blank_or_comment(line)) {
      const bool is_comment = indent < line.size() && line[indent] == '#';
      if (!is_comment || indent > parent_indent)
        continue;  // blank line, or comment inside the block: keep scanning
      break;  // outdented comment (section separator) — block ended
    }
    if (indent <= parent_indent)
      break;  // next key at/beyond parent indent — block ended
    if (!have_child_indent) {
      // First content line in the block sets the child indent (files use
      // parent+2 or deeper nesting; pyyaml seq items match sibling keys).
      child_indent = indent;
      have_child_indent = true;
    }
    insert_at = i + 1;
  }
  std::string new_line(child_indent, ' ');
  new_line += key + ":";
  if (!yaml.empty())
    new_line += " " + yaml;
  lines->insert(lines->begin() + static_cast<long>(insert_at), std::move(new_line));
  return insert_at;
}

// Newer settings that may be inserted when missing from an older file.
// Anything else that is absent keeps failing with "no config setting" so
// typos and removed keys are rejected instead of silently appended.
static bool insertable_config_path(const std::string &path) {
  return path == "config.buzzer.ring_at_start" || path == "config.buzzer.countdown.minutes_before" ||
         path == "config.buzzer.countdown.relays" ||
          path == "config.relays_enabled" || path == "config.relay_count" ||
          path == "config.log_level" ||
         path == "config.relay1_function" || path == "config.relay2_function" ||
         path == "config.relay3_function" || path == "config.relay4_function" ||
         path == "config.relay1_enabled" || path == "config.relay2_enabled" ||
         path == "config.relay3_enabled" || path == "config.relay4_enabled" ||
         path == "config.pregame_countdown" || path == "config.pregame_font_size" ||
         path == "config.allow_midnight_wrap" ||
         path == "config.pregame_color" || path == "config.pregame_header_text" ||
         path == "config.buzzer.start_duration" ||
         path == "config.buzzer.countdown_duration" ||
         path == "config.buzzer.end_duration" ||
         path == "config.footer.date_font_size" || path == "config.footer.date_color" ||
         path == "config.footer.time_font_size" || path == "config.footer.time_color" ||
         path == "config.footer.color" ||
         path == "config.background_color" ||
         path == "config.club_website_url" ||
         path == "config.league_page_url" ||
         path == "config.curling_club_manager_url";
}

// The countdown section needs both keys (minutes_before and relays), so a
// partial edit is completed from the current file: an unchanged key keeps its
// stored value, a section that does not exist yet starts at 0 minutes and no
// relays (event off).
static void complete_countdown_values(std::vector<ConfigValue> *values, const std::string &text) {
  bool has_minutes = false;
  bool has_relays = false;
  for (const ConfigValue &value : *values) {
    if (value.path == "config.buzzer.countdown.minutes_before")
      has_minutes = true;
    else if (value.path == "config.buzzer.countdown.relays")
      has_relays = true;
  }
  if (!has_minutes && !has_relays)
    return;  // no countdown edit: leave the section untouched
  int minutes = 0;
  std::vector<std::string> relays;
  PiTimerSettings settings;
  std::string err;
  if (settings.parse_settings_text(text, "settings.yml", &err)) {
    minutes = settings.countdown_minutes_before;
    relays = settings.countdown_relays;
  }
  if (!has_minutes)
    values->push_back(ConfigValue{"config.buzzer.countdown.minutes_before", yaml_int(minutes)});
  if (!has_relays)
    values->push_back(ConfigValue{"config.buzzer.countdown.relays", yaml_relays(relays)});
}

bool apply_config_values(const std::string &text, const std::vector<ConfigValue> &values,
                         std::string *out, std::string *err) {
  if (values.empty()) {
    *out = text;
    return true;
  }
  std::string newline;
  std::vector<std::string> lines = split_lines(text, &newline);

  std::vector<ConfigValue> complete = values;
  complete_countdown_values(&complete, text);

  for (const ConfigValue &value : complete) {
    // path must start with "config." and have at least one more segment
    if (value.path.rfind("config.", 0) != 0 || value.path.size() <= sizeof("config.") - 1) {
      *err = "no config setting '" + value.path + "' in the settings file";
      return false;
    }
    size_t index;
    if (!find_child_line(lines, -1, "config", &index)) {
      *err = "settings file has no top level 'config:' section";
      return false;
    }
    size_t rest = sizeof("config.") - 1;
    while (rest < value.path.size()) {
      size_t dot = value.path.find('.', rest);
      const std::string key =
          value.path.substr(rest, dot == std::string::npos ? std::string::npos : dot - rest);
      if (key.empty()) {
        *err = "no config setting '" + value.path + "' in the settings file";
        return false;
      }
      size_t child;
      if (!find_child_line(lines, static_cast<long>(index), key, &child)) {
        if (insertable_config_path(value.path)) {
          if (dot == std::string::npos) {
            // Newer setting missing from an older file: append it to the
            // parent block instead of failing the whole save.
            insert_child_value(&lines, index, key, value.yaml);
            break;
          }
          // Intermediate block missing (e.g. buzzer.countdown on older
          // files): create the empty parent line and keep walking the path.
          index = insert_child_value(&lines, index, key, "");
          rest = dot + 1;
          continue;
        }
        *err = "no config setting '" + value.path + "' in the settings file";
        return false;
      }
      if (dot == std::string::npos) {
        if (!replace_line_value(&lines, child, key, value.yaml)) {
          *err = "no config setting '" + value.path + "' in the settings file";
          return false;
        }
        break;
      }
      index = child;
      rest = dot + 1;
    }
  }

  out->clear();
  for (size_t i = 0; i < lines.size(); i++) {
    if (i > 0)
      *out += newline;
    *out += lines[i];
  }
  return true;
}

}  // namespace curling_engine
}  // namespace esphome
