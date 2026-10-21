#include "config_edit.h"

#include "model.h"

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

bool apply_config_values(const std::string &text, const std::vector<ConfigValue> &values,
                         std::string *out, std::string *err) {
  if (values.empty()) {
    *out = text;
    return true;
  }
  std::string newline;
  std::vector<std::string> lines = split_lines(text, &newline);

  for (const ConfigValue &value : values) {
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
