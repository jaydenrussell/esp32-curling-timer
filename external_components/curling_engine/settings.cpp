#include "settings.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace esphome {
namespace curling_engine {

// ===========================================================================
// YamlValue helpers
// ===========================================================================

const YamlValue *YamlValue::find(const std::string &key) const {
  if (type != YType::Map)
    return nullptr;
  for (const auto &kv : map)
    if (kv.first == key)
      return &kv.second;
  return nullptr;
}

std::string yaml_repr(const YamlValue &v) {
  switch (v.type) {
    case YType::Null:
      return "None";
    case YType::Bool:
      return v.b ? "True" : "False";
    case YType::Int: {
      char buf[32];
      snprintf(buf, sizeof(buf), "%lld", v.i);
      return buf;
    }
    case YType::Float: {
      char buf[40];
      snprintf(buf, sizeof(buf), "%.15g", v.d);
      return buf;
    }
    case YType::Str:
      return "'" + v.s + "'";
    case YType::Seq:
      return v.seq.empty() ? "[]" : "[...]";
    case YType::Map:
      return v.map.empty() ? "{}" : "{...}";
  }
  return "None";
}

static const char *type_name(YType t) {
  switch (t) {
    case YType::Map:
      return "dict";
    case YType::Seq:
      return "list";
    case YType::Int:
      return "int";
    case YType::Bool:
      return "bool";
    case YType::Str:
      return "str";
    default:
      return "None";
  }
}

// ===========================================================================
// Line-oriented parser for the pitimer YAML dialect
// ===========================================================================

namespace {

struct Parser {
  const std::string &text;
  std::vector<std::string> lines;
  size_t pos{0};
  std::string *err{nullptr};
  int line_no{1};
  bool failed{false};

  explicit Parser(const std::string &t) : text(t) {
    // split on \n, keep \r stripped (YAML parsers accept CRLF)
    std::string cur;
    for (char c : t) {
      if (c == '\n') {
        if (!cur.empty() && cur.back() == '\r')
          cur.pop_back();
        lines.push_back(cur);
        cur.clear();
      } else {
        cur.push_back(c);
      }
    }
    if (!cur.empty() && cur.back() == '\r')
      cur.pop_back();
    lines.push_back(cur);  // final line (may be empty)
  }

  bool fail(const std::string &msg) {
    if (!failed) {
      failed = true;
      *err = "line " + std::to_string(line_no) + ": " + msg;
    }
    return false;
  }

  static bool only_ws(const std::string &s) {
    return s.find_first_not_of(" \t") == std::string::npos;
  }

  static int indent_of(const std::string &s) {
    int n = 0;
    for (char c : s) {
      if (c == ' ')
        n++;
      else if (c == '\t')
        return -1;  // tab indentation is an error
      else
        break;
    }
    return n;
  }

  // true when the line is blank or a full-line comment
  static bool trivia(const std::string &s) {
    if (only_ws(s))
      return true;
    size_t i = s.find_first_not_of(" \t");
    return i != std::string::npos && s[i] == '#';
  }

  // advance pos over blank/comment lines; checks tab indentation
  bool skip_trivia() {
    while (pos < lines.size()) {
      const std::string &l = lines[pos];
      if (indent_of(l) < 0)
        return fail("found character '\\t' that cannot start any token");
      if (!trivia(l))
        return true;
      pos++;
    }
    return true;
  }

  // -------------------------------------------------- scalars ------------

  // Strip an inline comment from a non-quoted remainder: ' #' starts one.
  static size_t comment_pos(const std::string &s) {
    for (size_t i = 0; i < s.size(); i++) {
      if (s[i] == '#' && (i == 0 || s[i - 1] == ' ' || s[i - 1] == '\t'))
        return i;
    }
    return std::string::npos;
  }

  static std::string strip_ws(const std::string &s) {
    size_t a = s.find_first_not_of(" \t");
    if (a == std::string::npos)
      return "";
    size_t b = s.find_last_not_of(" \t");
    return s.substr(a, b - a + 1);
  }

  // YAML 1.1 type resolution for a plain (unquoted) scalar.
  static void resolve(const std::string &raw_in, YamlValue *out) {
    std::string raw = strip_ws(raw_in);
    out->type = YType::Null;
    out->s = raw;
    if (raw.empty() || raw == "~" || raw == "null" || raw == "Null" || raw == "NULL") {
      out->type = YType::Null;
      return;
    }
    // bools (PyYAML SafeLoader resolver list)
    if (raw == "true" || raw == "True" || raw == "TRUE") {
      out->type = YType::Bool;
      out->b = true;
      return;
    }
    if (raw == "false" || raw == "False" || raw == "FALSE" || raw == "no" || raw == "No" ||
        raw == "NO" || raw == "off" || raw == "Off" || raw == "OFF") {
      out->type = YType::Bool;
      out->b = false;
      return;
    }
    if (raw == "yes" || raw == "Yes" || raw == "YES" || raw == "on" || raw == "On" ||
        raw == "ON") {
      out->type = YType::Bool;
      out->b = true;
      return;
    }
    if (parse_number(raw, out))
      return;
    out->type = YType::Str;
  }

  // int (decimal/hex/octal/sexagesimal, YAML 1.1) or float; false => plain str
  static bool parse_number(const std::string &raw, YamlValue *out) {
    if (raw.empty())
      return false;
    const char *s = raw.c_str();
    bool neg = false;
    size_t i = 0;
    if (s[0] == '+' || s[0] == '-') {
      neg = s[0] == '-';
      i = 1;
    }
    if (i >= raw.size())
      return false;

    // sexagesimal: [1-9][0-9_]*(:[0-5]?[0-9])+  (no leading zero, PyYAML)
    if (std::isdigit(static_cast<unsigned char>(s[i])) && s[i] != '0') {
      size_t j = i;
      bool digits = false;
      while (j < raw.size() && (std::isdigit(static_cast<unsigned char>(s[j])) || s[j] == '_')) {
        digits = true;
        j++;
      }
      if (digits && j < raw.size() && s[j] == ':') {
        size_t k = j;
        long long value = 0;
        bool ok = true;
        // first component: digits collected in [i, j)
        for (size_t p = i; p < j; p++) {
          if (s[p] == '_')
            continue;
          value = value * 10 + (s[p] - '0');
        }
        while (k < raw.size() && s[k] == ':') {
          size_t m = k + 1;
          long long comp = 0;
          int count = 0;
          while (m < raw.size() && std::isdigit(static_cast<unsigned char>(s[m]))) {
            comp = comp * 10 + (s[m] - '0');
            m++;
            count++;
          }
          if (count == 0 || count > 2 || comp > 59) {
            ok = false;
            break;
          }
          if (count == 2 && (s[k + 1] < '0' || s[k + 1] > '5')) {
            ok = false;
            break;
          }
          value = value * 60 + comp;
          k = m;
        }
        if (ok && k == raw.size()) {
          out->type = YType::Int;
          out->i = neg ? -value : value;
          return true;
        }
        // matched the leading pattern but trailing garbage: fall through
        if (k == raw.size())
          return false;
      }
    }

    // hex
    if (raw.size() > i + 1 && s[i] == '0' && (s[i + 1] == 'x' || s[i + 1] == 'X')) {
      long long value = 0;
      for (size_t p = i + 2; p < raw.size(); p++) {
        char c = s[p];
        if (c == '_')
          continue;
        int d;
        if (c >= '0' && c <= '9')
          d = c - '0';
        else if (c >= 'a' && c <= 'f')
          d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
          d = c - 'A' + 10;
        else
          return false;
        value = value * 16 + d;
      }
      out->type = YType::Int;
      out->i = neg ? -value : value;
      return true;
    }
    // octal (YAML 1.1 implicit, leading 0)
    if (raw.size() > i + 1 && s[i] == '0' && std::isdigit(static_cast<unsigned char>(s[i + 1]))) {
      bool octal = true;
      long long value = 0;
      for (size_t p = i; p < raw.size(); p++) {
        char c = s[p];
        if (c == '_')
          continue;
        if (c < '0' || c > '7') {
          octal = false;
          break;
        }
        value = value * 8 + (c - '0');
      }
      if (octal) {
        out->type = YType::Int;
        out->i = neg ? -value : value;
        return true;
      }
      // e.g. 08: not octal, not decimal (leading zero) => plain string (PyYAML)
      return false;
    }

    // decimal int (no underscores except between digits; no leading zeros)
    bool all_digits = true;
    size_t start = i;
    if (i >= raw.size())
      return false;
    if (s[i] == '0' && raw.size() - i > 1 && std::isdigit(static_cast<unsigned char>(s[1])))
      return false;  // leading zero with more digits, not octal => str
    for (size_t p = i; p < raw.size(); p++) {
      char c = s[p];
      if (c == '_')
        continue;
      if (!std::isdigit(static_cast<unsigned char>(c))) {
        all_digits = false;
        break;
      }
    }
    if (all_digits && raw.size() - start > 0) {
      errno = 0;
      char *end = nullptr;
      std::string digits;
      for (size_t p = i; p < raw.size(); p++)
        if (s[p] != '_')
          digits.push_back(s[p]);
      long long value = strtoll(digits.c_str(), &end, 10);
      if (end && *end == '\0') {
        out->type = YType::Int;
        out->i = neg ? -value : value;
        return true;
      }
      return false;
    }

    // float: digits with optional '.' and exponent, or .5 / 5. form
    {
      size_t p = i;
      bool saw_digit = false, saw_dot = false;
      while (p < raw.size()) {
        char c = s[p];
        if (std::isdigit(static_cast<unsigned char>(c))) {
          saw_digit = true;
          p++;
        } else if (c == '.' && !saw_dot) {
          saw_dot = true;
          p++;
        } else if ((c == 'e' || c == 'E') && saw_digit) {
          p++;
          if (p < raw.size() && (s[p] == '+' || s[p] == '-'))
            p++;
          bool expdig = false;
          while (p < raw.size() && std::isdigit(static_cast<unsigned char>(s[p]))) {
            expdig = true;
            p++;
          }
          if (!expdig)
            return false;
          break;
        } else if (c == '_') {
          p++;
        } else {
          return false;
        }
      }
      if (p == raw.size() && saw_digit && (saw_dot || raw.find('e') != std::string::npos ||
                                            raw.find('E') != std::string::npos)) {
        out->type = YType::Float;
        errno = 0;
        out->d = strtod(raw.c_str(), nullptr);
        return true;
      }
      if (p == raw.size() && saw_digit && !saw_dot) {
        // pure int digits already handled above; if we got here the int path
        // rejected (e.g. leading zero) => string
        return false;
      }
    }
    return false;
  }

  // unescape a double-quoted scalar body; returns false on bad escape
  static bool unescape_double(const std::string &body, std::string *out) {
    out->clear();
    for (size_t i = 0; i < body.size(); i++) {
      char c = body[i];
      if (c != '\\') {
        out->push_back(c);
        continue;
      }
      if (++i >= body.size())
        return false;
      char e = body[i];
      switch (e) {
        case 'n':
          out->push_back('\n');
          break;
        case 't':
          out->push_back('\t');
          break;
        case 'r':
          out->push_back('\r');
          break;
        case '0':
          out->push_back('\0');
          break;
        case '\\':
          out->push_back('\\');
          break;
        case '"':
          out->push_back('"');
          break;
        case '/':
          out->push_back('/');
          break;
        case 'u': {
          if (i + 4 >= body.size())
            return false;
          unsigned code = 0;
          for (int k = 0; k < 4; k++) {
            char h = body[++i];
            code <<= 4;
            if (h >= '0' && h <= '9')
              code |= h - '0';
            else if (h >= 'a' && h <= 'f')
              code |= h - 'a' + 10;
            else if (h >= 'A' && h <= 'F')
              code |= h - 'A' + 10;
            else
              return false;
          }
          // utf-8 encode (BMP only)
          if (code < 0x80) {
            out->push_back(static_cast<char>(code));
          } else if (code < 0x800) {
            out->push_back(static_cast<char>(0xC0 | (code >> 6)));
            out->push_back(static_cast<char>(0x80 | (code & 0x3F)));
          } else {
            out->push_back(static_cast<char>(0xE0 | (code >> 12)));
            out->push_back(static_cast<char>(0x80 | ((code >> 6) & 0x3F)));
            out->push_back(static_cast<char>(0x80 | (code & 0x3F)));
          }
          break;
        }
        default:
          return false;
      }
    }
    return true;
  }

  // Parse a single-line quoted scalar starting at s[i] == quote; returns the
  // index just past the closing quote (or npos).
  static size_t scan_quoted(const std::string &s, size_t i) {
    char q = s[i];
    for (size_t j = i + 1; j < s.size(); j++) {
      if (q == '\'' ) {
        if (s[j] == '\'') {
          if (j + 1 < s.size() && s[j + 1] == '\'') {
            j++;
            continue;
          }
          return j + 1;
        }
      } else {
        if (s[j] == '\\') {
          j++;
          continue;
        }
        if (s[j] == '"')
          return j + 1;
      }
    }
    return std::string::npos;
  }

  // -------------------------------------------------- flow ---------------

  // gather a flow collection that may span lines; returns the full text from
  // offset start (within line `p_line`) or fails
  bool gather_flow(size_t p_line, size_t start, std::string *out) {
    int depth = 0;
    bool in_s = false, in_d = false;
    std::string acc;
    size_t line = p_line;
    size_t begin = start;
    for (;;) {
      const std::string &l = lines[line];
      for (size_t i = begin; i < l.size(); i++) {
        char c = l[i];
        if (in_s) {
          acc.push_back(c);
          if (c == '\'') {
            if (i + 1 < l.size() && l[i + 1] == '\'') {
              i++;
              acc.push_back(l[i]);  // keep the '' escape pair for parse_flow
            } else {
              in_s = false;
            }
          }
          continue;
        }
        if (in_d) {
          acc.push_back(c);
          if (c == '\\' && i + 1 < l.size()) {
            acc.push_back(l[++i]);
          } else if (c == '"') {
            in_d = false;
          }
          continue;
        }
        if (c == '\'' && depth > 0) {
          in_s = true;
          acc.push_back(c);
        } else if (c == '"' && depth > 0) {
          in_d = true;
          acc.push_back(c);
        } else if (c == '[' || c == '{') {
          depth++;
          acc.push_back(c);
        } else if (c == ']' || c == '}') {
          depth--;
          acc.push_back(c);
          if (depth == 0) {
            // rest of line must be blank or comment
            std::string rest = l.substr(i + 1);
            size_t cp = comment_pos(rest);
            if (cp != std::string::npos)
              rest = rest.substr(0, cp);
            if (!only_ws(rest))
              return fail("unexpected content after flow collection");
            *out = acc;
            pos = line + 1;
            line_no = static_cast<int>(line) + 1;
            return true;
          }
        } else {
          acc.push_back(c);
        }
      }
      line++;
      if (line >= lines.size())
        return fail("unterminated flow collection");
      acc.push_back('\n');
      begin = 0;
      line_no = static_cast<int>(line) + 1;
    }
  }

  static void skip_ws(const std::string &s, size_t *p) {
    while (*p < s.size() &&
           (s[*p] == ' ' || s[*p] == '\t' || s[*p] == '\n' || s[*p] == '\r'))
      (*p)++;
  }

  bool parse_flow(const std::string &s, size_t *p, YamlValue *out) {
    skip_ws(s, p);
    if (*p >= s.size())
      return fail("flow: unexpected end");
    char c = s[*p];
    if (c == '[') {
      out->type = YType::Seq;
      (*p)++;
      skip_ws(s, p);
      if (*p < s.size() && s[*p] == ']') {
        (*p)++;
        return true;
      }
      for (;;) {
        YamlValue item;
        if (!parse_flow(s, p, &item))
          return false;
        out->seq.push_back(std::move(item));
        skip_ws(s, p);
        if (*p >= s.size())
          return fail("flow: unterminated sequence");
        if (s[*p] == ',') {
          (*p)++;
          skip_ws(s, p);
          if (*p < s.size() && s[*p] == ']') {  // trailing comma
            (*p)++;
            return true;
          }
          continue;
        }
        if (s[*p] == ']') {
          (*p)++;
          return true;
        }
        return fail("flow: expected ',' or ']'");
      }
    }
    if (c == '{') {
      out->type = YType::Map;
      (*p)++;
      skip_ws(s, p);
      if (*p < s.size() && s[*p] == '}') {
        (*p)++;
        return true;
      }
      for (;;) {
        // key
        std::string key;
        skip_ws(s, p);
        if (*p >= s.size())
          return fail("flow: unterminated mapping");
        if (s[*p] == '\'' || s[*p] == '"') {
          size_t end = scan_quoted(s, *p);
          if (end == std::string::npos)
            return fail("flow: unterminated quoted key");
          std::string body = s.substr(*p + 1, end - *p - 2);
          if (s[*p] == '\'') {
            key.clear();
            for (size_t i = 0; i < body.size(); i++) {
              if (body[i] == '\'' && i + 1 < body.size() && body[i + 1] == '\'') {
                key.push_back('\'');
                i++;
              } else {
                key.push_back(body[i]);
              }
            }
          } else if (!unescape_double(body, &key)) {
            return fail("flow: bad escape in key");
          }
          *p = end;
        } else {
          size_t start = *p;
          while (*p < s.size()) {
            char ch = s[*p];
            if (ch == ':' && (*p + 1 >= s.size() || s[*p + 1] == ' ' || s[*p + 1] == '\n' ||
                              s[*p + 1] == '\t' || s[*p + 1] == '\r'))
              break;
            if (ch == ',' || ch == '}' || ch == '\n')
              break;
            (*p)++;
          }
          key = strip_ws(s.substr(start, *p - start));
        }
        skip_ws(s, p);
        if (*p >= s.size() || s[*p] != ':')
          return fail("flow: expected ':' after key '" + key + "'");
        (*p)++;  // ':'
        YamlValue val;
        if (!parse_flow(s, p, &val))
          return false;
        out->map.emplace_back(key, std::move(val));
        skip_ws(s, p);
        if (*p >= s.size())
          return fail("flow: unterminated mapping");
        if (s[*p] == ',') {
          (*p)++;
          skip_ws(s, p);
          if (*p < s.size() && s[*p] == '}') {
            (*p)++;
            return true;
          }
          continue;
        }
        if (s[*p] == '}') {
          (*p)++;
          return true;
        }
        return fail("flow: expected ',' or '}'");
      }
    }
    if (c == '\'' || c == '"') {
      size_t end = scan_quoted(s, *p);
      if (end == std::string::npos)
        return fail("flow: unterminated quoted scalar");
      std::string body = s.substr(*p + 1, end - *p - 2);
      out->type = YType::Str;
      if (c == '\'') {
        out->s.clear();
        for (size_t i = 0; i < body.size(); i++) {
          if (body[i] == '\'' && i + 1 < body.size() && body[i + 1] == '\'') {
            out->s.push_back('\'');
            i++;
          } else {
            out->s.push_back(body[i]);
          }
        }
      } else if (!unescape_double(body, &out->s)) {
        return fail("flow: bad escape in scalar");
      }
      *p = end;
      return true;
    }
    // plain scalar in flow: ends at , ] } or ': ' (or newline)
    size_t start = *p;
    while (*p < s.size()) {
      char ch = s[*p];
      if (ch == ',' || ch == ']' || ch == '}' || ch == '\n' || ch == '\r')
        break;
      if (ch == ':' && (*p + 1 >= s.size() || s[*p + 1] == ' ' || s[*p + 1] == '\n' ||
                        s[*p + 1] == '\t'))
        break;
      (*p)++;
    }
    std::string raw = s.substr(start, *p - start);
    // trailing whitespace belongs to the delimiter, not the scalar
    size_t end = raw.find_last_not_of(" \t\r");
    raw = end == std::string::npos ? "" : raw.substr(0, end + 1);
    if (raw.empty())
      return fail("flow: empty scalar");
    resolve(raw, out);
    return true;
  }

  // -------------------------------------------------- block --------------

  // split "key: rest" at the first ': ' (or ':' at EOL) outside quotes;
  // returns index of ':' or npos
  static size_t find_key_colon(const std::string &s, size_t begin) {
    if (begin >= s.size())
      return std::string::npos;
    if (s[begin] == '\'' || s[begin] == '"') {
      size_t end = scan_quoted(s, begin);
      if (end == std::string::npos)
        return std::string::npos;
      size_t i = end;
      while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
        i++;
      if (i < s.size() && s[i] == ':')
        return i;
      return std::string::npos;
    }
    for (size_t i = begin; i < s.size(); i++) {
      if (s[i] == '#') {
        // comment starts if preceded by whitespace
        if (i == begin)
          return std::string::npos;
        if (s[i - 1] == ' ' || s[i - 1] == '\t')
          return std::string::npos;
      }
      if (s[i] == ':' && (i + 1 >= s.size() || s[i + 1] == ' ' || s[i + 1] == '\t'))
        return i;
    }
    return std::string::npos;
  }

  static std::string unquote_plain_key(const std::string &body) {
    return strip_ws(body);
  }

  // Read a block scalar ("|" or ">" with chomp) whose indicator was in the
  // line at parent_line at column; consumes following deeper lines.
  bool read_block_scalar(size_t parent_line, int parent_indent, const std::string &indicator,
                         std::string *out) {
    char style = indicator[0];
    char chomp = indicator.size() > 1 ? indicator[1] : ' ';
    // content lines: blank or indent > parent_indent
    std::vector<std::string> content;
    int content_indent = -1;
    size_t line = parent_line + 1;
    while (line < lines.size()) {
      const std::string &l = lines[line];
      int ind = indent_of(l);
      if (ind < 0)
        return fail("found character '\\t' that cannot start any token");
      if (only_ws(l)) {
        content.push_back("");
        line++;
        continue;
      }
      if (ind <= parent_indent)
        break;
      if (content_indent < 0)
        content_indent = ind;
      if (ind < content_indent)
        return fail("block scalar line is indented less than the first line");
      content.push_back(l.substr(content_indent));
      line++;
    }
    // trailing blanks beyond content belong to chomping; trim them for now
    size_t real_end = content.size();
    while (real_end > 0 && content[real_end - 1].empty())
      real_end--;
    size_t trailing_blanks = content.size() - real_end;
    content.resize(real_end);

    out->clear();
    if (style == '|') {
      for (size_t i = 0; i < content.size(); i++) {
        out->append(content[i]);
        out->push_back('\n');
      }
      if (chomp == '-')
        while (!out->empty() && out->back() == '\n')
          out->pop_back();
      // clip (default) and keep: content already ends with one '\n'
      if (chomp == '+')
        for (size_t i = 0; i < trailing_blanks; i++)
          out->push_back('\n');
    } else {  // '>' fold
      bool prev_blank = false;
      bool first = true;
      for (const std::string &l : content) {
        if (l.empty()) {
          out->push_back('\n');
          prev_blank = true;
          first = false;
          continue;
        }
        if (!first && !prev_blank)
          out->push_back(' ');
        else if (prev_blank && !out->empty() && out->back() != '\n')
          out->push_back('\n');
        out->append(l);
        prev_blank = false;
        first = false;
      }
      if (chomp != '-' && !out->empty() && out->back() != '\n')
        out->push_back('\n');
      if (chomp == '-' || chomp == ' ') {
        while (!out->empty() && out->back() == '\n')
          out->pop_back();
        if (chomp == ' ' && !out->empty())
          out->push_back('\n');
      }
      if (chomp == '+')
        for (size_t i = 0; i < trailing_blanks; i++)
          out->push_back('\n');
    }
    pos = line;
    return true;
  }

  // Parse the inline value that sits on a key/item line (rest after ':').
  // base: absolute column of rest[0] within its line; *advance receives the
  // parser position after this line (past block scalar content when used).
  bool parse_inline(const std::string &rest_in, size_t parent_line, int parent_indent,
                    size_t base, size_t *advance, YamlValue *out, bool *has_inline) {
    *has_inline = false;
    *advance = parent_line + 1;
    std::string rest = rest_in;
    // cut comment (quote aware enough: comment only when preceded by ws)
    size_t cp = comment_pos(rest);
    if (cp != std::string::npos)
      rest = rest.substr(0, cp);
    rest = strip_ws(rest);
    if (rest.empty()) {
      out->type = YType::Null;
      return true;  // no inline value (nested block may follow)
    }
    *has_inline = true;
    if (rest[0] == '&' || rest[0] == '*' || rest[0] == '!')
      return fail("anchors, aliases and tags are not supported");
    if (rest[0] == '|' || rest[0] == '>') {
      std::string indicator = rest;
      if (indicator.size() > 2)
        return fail("bad block scalar indicator");
      if (indicator.size() == 2 && indicator[1] != '-' && indicator[1] != '+')
        return fail("bad block scalar indicator");
      out->type = YType::Str;
      if (!read_block_scalar(parent_line, parent_indent, indicator, &out->s))
        return false;
      *advance = pos;
      return true;
    }
    if (rest[0] == '[' || rest[0] == '{') {
      std::string flow_text;
      if (!gather_flow(parent_line, base + strip_left_len(rest_in), &flow_text))
        return false;
      size_t p = flow_text.find_first_of("[{");
      if (!parse_flow(flow_text, &p, out))
        return false;
      *advance = pos;
      return true;
    }
    if (rest[0] == '\'' || rest[0] == '"') {
      // quoted scalar on one line (rest already comment-stripped)
      size_t end = scan_quoted(rest, 0);
      if (end == std::string::npos)
        return fail("unterminated quoted scalar");
      std::string after = strip_ws(rest.substr(end));
      if (!after.empty() && after[0] != '#')
        return fail("unexpected content after quoted scalar");
      std::string body = rest.substr(1, end - 2);
      out->type = YType::Str;
      if (rest[0] == '\'') {
        out->s.clear();
        for (size_t i = 0; i < body.size(); i++) {
          if (body[i] == '\'' && i + 1 < body.size() && body[i + 1] == '\'') {
            out->s.push_back('\'');
            i++;
          } else {
            out->s.push_back(body[i]);
          }
        }
      } else if (!unescape_double(body, &out->s)) {
        return fail("bad escape in quoted scalar");
      }
      return true;
    }
    resolve(rest, out);
    return true;
  }

  // offset of first non-space in s
  static size_t strip_left_len(const std::string &s) {
    size_t i = s.find_first_not_of(" \t");
    return i == std::string::npos ? s.size() : i;
  }

  // Parse a block mapping starting at pos with the given indent.
  // first_offset: when >= 0, the line at pos is a compact seq item whose key
  // begins at that column instead of its physical indent.
  bool parse_map_block(int indent, YamlValue *out, int first_offset = -1) {
    out->type = YType::Map;
    bool first = first_offset >= 0;
    while (true) {
      // trivia between entries
      while (pos < lines.size() && trivia(lines[pos])) {
        if (indent_of(lines[pos]) < 0)
          return fail("found character '\\t' that cannot start any token");
        pos++;
      }
      if (pos >= lines.size())
        return true;
      const std::string &line = lines[pos];
      int phys = indent_of(line);
      if (phys < 0)
        return fail("found character '\\t' that cannot start any token");
      size_t key_begin;
      if (first) {
        key_begin = static_cast<size_t>(first_offset);
        first = false;
      } else {
        if (phys < indent)
          return true;  // block ended (less-indented line belongs to parent)
        if (phys > indent)
          return fail("bad indentation (expected " + std::to_string(indent) + " spaces)");
        key_begin = static_cast<size_t>(phys);
      }
      // split key / rest
      size_t colon = find_key_colon(line, key_begin);
      if (colon == std::string::npos)
        return fail("could not find expected ':'");
      std::string key_raw = line.substr(key_begin, colon - key_begin);
      std::string key = unquote_plain_key(key_raw);
      if (key.empty())
        return fail("empty mapping key");
      if (key.size() >= 2 && (key.front() == '\'' || key.front() == '"')) {
        // quoted key: unquote
        YamlValue kv;
        size_t p = 0;
        if (!parse_flow(key, &p, &kv) || kv.type != YType::Str)
          return fail("bad quoted key");
        key = kv.s;
      }
      std::string rest = line.substr(colon + 1);
      YamlValue value;
      bool has_inline = false;
      size_t this_line = pos;
      size_t adv = this_line + 1;
      if (!parse_inline(rest, this_line, indent, colon + 1, &adv, &value, &has_inline))
        return false;
      if (!has_inline) {
        // nested block: next non-trivia line deeper than indent, or a block
        // sequence entry at the SAME indent as its parent key (PyYAML's
        // default style: "key:\n- item")
        // (pos is still this_line; parse_inline only reported advance)
        size_t look = this_line + 1;
        while (look < lines.size() && trivia(lines[look]))
          look++;
        if (look < lines.size()) {
          int li = indent_of(lines[look]);
          if (li < 0)
            return fail("found character '\\t' that cannot start any token");
          bool seq_line = false;
          if (li >= 0 && lines[look].size() > static_cast<size_t>(li) &&
              lines[look][static_cast<size_t>(li)] == '-') {
            size_t k = static_cast<size_t>(li) + 1;
            seq_line = k >= lines[look].size() || lines[look][k] == ' ' || lines[look][k] == '\t';
          }
          if (li > indent || (li == indent && seq_line)) {
            pos = look;
            int child = li;
            if (!parse_block(child, &value))
              return false;
          } else {
            pos = this_line + 1;
            value.type = YType::Null;
          }
        } else {
          pos = lines.size();
          value.type = YType::Null;
        }
      } else {
        pos = adv;
        // a deeper non-trivia line after an inline scalar would be a
        // multi-line plain scalar, which this parser does not support
        size_t look = pos;
        while (look < lines.size() && trivia(lines[look]))
          look++;
        if (look < lines.size()) {
          int li = indent_of(lines[look]);
          if (li < 0)
            return fail("found character '\\t' that cannot start any token");
          if (li > indent)
            return fail(
                "multi-line plain scalars are not supported; quote the text or use '|' "
                "block scalars");
        }
      }
      out->map.emplace_back(key, std::move(value));
    }
  }

  bool parse_seq_block(int indent, YamlValue *out) {
    out->type = YType::Seq;
    while (true) {
      while (pos < lines.size() && trivia(lines[pos])) {
        if (indent_of(lines[pos]) < 0)
          return fail("found character '\\t' that cannot start any token");
        pos++;
      }
      if (pos >= lines.size())
        return true;
      const std::string &line = lines[pos];
      int phys = indent_of(line);
      if (phys < 0)
        return fail("found character '\\t' that cannot start any token");
      if (phys < indent)
        return true;
      if (phys > indent)
        return fail("bad indentation in sequence (expected " + std::to_string(indent) +
                    " spaces)");
      size_t i = static_cast<size_t>(phys);
      // a non-entry line at this indent belongs to the enclosing block (block
      // sequences may sit at the same indent as their parent mapping key, so
      // their entries end where the parent's next key begins)
      if (line.size() <= i || line[i] != '-')
        return true;
      if (line.size() > i + 1 && line[i + 1] != ' ' && line[i + 1] != '\t')
        return true;
      size_t rest_at = i + 1;
      while (rest_at < line.size() && (line[rest_at] == ' ' || line[rest_at] == '\t'))
        rest_at++;
      std::string rest = line.substr(rest_at);
      if (trivia(rest)) {
        // nested block item
        size_t look = pos + 1;
        while (look < lines.size() && trivia(lines[look]))
          look++;
        if (look < lines.size() && indent_of(lines[look]) > indent) {
          pos = look;
          YamlValue item;
          if (!parse_block(indent_of(lines[look]), &item))
            return false;
          out->seq.push_back(std::move(item));
          continue;
        }
        pos++;
        YamlValue null;
        null.type = YType::Null;
        out->seq.push_back(std::move(null));
        continue;
      }
      // item content on the same line
      std::string trimmed = strip_ws(rest);
      // flow collection item: never a compact mapping even if it contains ':'
      bool flow_item = !trimmed.empty() && (trimmed[0] == '[' || trimmed[0] == '{');
      // compact mapping item: has a key: value on this line?
      size_t colon = flow_item ? std::string::npos : find_key_colon(rest, 0);
      bool compact = colon != std::string::npos;
      YamlValue item;
      if (compact) {
        int key_col = static_cast<int>(rest_at + strip_left_len(rest));
        if (!parse_map_block(key_col, &item, key_col))
          return false;
      } else {
        bool has_inline = false;
        size_t this_line = pos;
        size_t adv = this_line + 1;
        if (!parse_inline(rest, this_line, indent, rest_at, &adv, &item, &has_inline))
          return false;
        pos = adv;
      }
      out->seq.push_back(std::move(item));
    }
  }

  bool parse_block(int indent, YamlValue *out) {
    if (pos >= lines.size()) {
      out->type = YType::Null;
      return true;
    }
    // decide seq vs map from first non-trivia line at >= indent
    size_t look = pos;
    while (look < lines.size() && trivia(lines[look]))
      look++;
    if (look >= lines.size()) {
      pos = look;
      out->type = YType::Null;
      return true;
    }
    const std::string &l = lines[look];
    int li = indent_of(l);
    if (li < 0)
      return fail("found character '\\t' that cannot start any token");
    if (li < indent) {
      pos = look;
      out->type = YType::Null;
      return true;
    }
    pos = look;
    size_t i = static_cast<size_t>(li);
    bool is_seq = l.size() > i && l[i] == '-' &&
                  (l.size() == i + 1 || l[i + 1] == ' ' || l[i + 1] == '\t');
    if (is_seq)
      return parse_seq_block(li, out);
    return parse_map_block(li, out);
  }
};

}  // namespace

bool yaml_parse(const std::string &text, YamlValue *out, std::string *err) {
  std::string clean = text;
  // strip UTF-8 BOM (PyYAML accepts one)
  if (clean.size() >= 3 && clean[0] == '\xEF' && clean[1] == '\xBB' && clean[2] == '\xBF')
    clean.erase(0, 3);
  Parser p(clean);
  p.err = err;
  p.line_no = 1;
  YamlValue root;
  if (!p.skip_trivia())
    return false;
  if (p.pos >= p.lines.size()) {
    root.type = YType::Null;
    *out = std::move(root);
    return true;
  }
  // optional document start marker
  if (p.lines[p.pos] == "---") {
    p.pos++;
    if (!p.skip_trivia())
      return false;
    if (p.pos >= p.lines.size()) {
      out->type = YType::Null;
      return true;
    }
  }
  if (!p.parse_block(0, &root))
    return false;
  if (!p.skip_trivia())
    return false;
  if (p.pos < p.lines.size())
    return p.fail("content after document");
  *out = std::move(root);
  return true;
}

// ===========================================================================
// Validation (port of PiTimerSettings; messages verbatim from settings.py)
// ===========================================================================

// Port of settings.py _require: section must be a map, key must exist and
// hold the expected type ("bool is not an int" is covered by YType identity).
static const YamlValue *require_(const YamlValue *section, const char *key, YType expected,
                                 const char *where, std::string *err) {
  if (section == nullptr || section->type != YType::Map) {
    *err = std::string(where) + " is missing or not a section";
    return nullptr;
  }
  const YamlValue *v = section->find(key);
  if (v == nullptr) {
    *err = std::string("missing '") + key + "' in " + where;
    return nullptr;
  }
  if (v->type != expected) {
    *err = std::string("'") + key + "' in " + where + " is not a valid " +
           type_name(expected) + " (" + yaml_repr(*v) + ")";
    return nullptr;
  }
  return v;
}

// Port of the buzzer-section relay list validation (settings.py: relays must
// be a list of RELAY1..RELAY4). Also used for the optional ring_at_start and
// countdown.relays lists.
static bool parse_relay_seq(const YamlValue &seq, const char *what, const char *where,
                            std::vector<std::string> *out, std::string *err) {
  out->clear();
  for (const YamlValue &relay : seq.seq) {
    bool ok = relay.type == YType::Str && (relay.s == "RELAY1" || relay.s == "RELAY2" ||
                                           relay.s == "RELAY3" || relay.s == "RELAY4");
    if (!ok) {
      *err = std::string("entry ") + yaml_repr(relay) + " in " + where + " " + what +
             " not one of " + VALID_RELAYS_REPR;
      return false;
    }
    out->push_back(relay.s);
  }
  return true;
}

// Python f-string helper: None prints bare 'None' inside messages.
static std::string game_msg(const PiTimerGame &g) {
  return g.message.has_value() ? *g.message : "None";
}

// Port of PiTimerSettings.parse_schedules: validate one day->entries map.
static bool parse_schedules_into(const YamlValue &src,
                                 std::map<std::string, std::vector<PiTimerGame>> *dest,
                                 int default_header_hold, std::string *err) {
  dest->clear();
  auto hold = [default_header_hold](const PiTimerGame &g) {
    return g.header_hold_override.has_value() ? *g.header_hold_override : default_header_hold;
  };
  for (const auto &day_kv : src.map) {
    const std::string &day = day_kv.first;
    const YamlValue &entries = day_kv.second;
    if (entries.type != YType::Seq) {
      *err = day + " must be a list of entries (use '[]' for none)";
      return false;
    }
    std::vector<PiTimerGame> list;
    for (const YamlValue &entry_v : entries.seq) {
      std::string where = "a schedule entry for " + day + " (leading zero maybe?)";
      const YamlValue *sv = require_(&entry_v, "start", YType::Int, where.c_str(), err);
      if (sv == nullptr)
        return false;
      PiTimerGame g;
      g.start = static_cast<int>(sv->i);
      const YamlValue *bv = require_(&entry_v, "buzzer", YType::Int, where.c_str(), err);
      if (bv == nullptr)
        return false;
      g.buzzer = static_cast<int>(bv->i);
      if (entry_v.has("name")) {
        const YamlValue *v = require_(&entry_v, "name", YType::Str, where.c_str(), err);
        if (v == nullptr)
          return false;
        g.message = v->s;
      }
      if (entry_v.has("during_buzzer")) {
        const YamlValue *v = require_(&entry_v, "during_buzzer", YType::Str, where.c_str(), err);
        if (v == nullptr)
          return false;
        g.during_buzzer_override = v->s;
      }
      if (entry_v.has("header_hold")) {
        const YamlValue *v = require_(&entry_v, "header_hold", YType::Int, where.c_str(), err);
        if (v == nullptr)
          return false;
        g.header_hold_override = static_cast<int>(v->i);
        if (*g.header_hold_override < 0) {
          *err = "'header_hold' cannot be negative for " + day;
          return false;
        }
      }
      if (entry_v.has("pace_of_play")) {
        const YamlValue *v = require_(&entry_v, "pace_of_play", YType::Bool, where.c_str(), err);
        if (v == nullptr)
          return false;
        g.pace_of_play_override = v->b;
      }
      if (entry_v.has("pace_unit_duration")) {
        const YamlValue *v =
            require_(&entry_v, "pace_unit_duration", YType::Int, where.c_str(), err);
        if (v == nullptr)
          return false;
        g.pace_unit_override = static_cast<int>(v->i);
        if (*g.pace_unit_override <= 0) {
          *err = "'pace_unit_duration' must be positive for " + day;
          return false;
        }
      }

      int duration = g.buzzer - g.start;
      if (duration <= 0) {
        *err = "start after buzzer for " + day + " " + game_msg(g) + " entry";
        return false;
      }
      if (duration >= MAX_GAME_SECONDS) {
        *err = "time between start and buzzer for " + day + " " + game_msg(g) +
               " invalid (>3 hours)";
        return false;
      }

      // two intervals overlap iff each starts before the other ends, each end
      // counting that entry's own header hold
      int new_end = g.buzzer + hold(g);
      for (const PiTimerGame &cmp : list) {
        int cmp_end = cmp.buzzer + hold(cmp);
        if (g.start <= cmp_end && cmp.start <= new_end) {
          *err = day + " " + game_msg(g) + " overlaps " + game_msg(cmp) + " (including the " +
                 std::to_string(hold(g)) + "s and " + std::to_string(hold(cmp)) +
                 "s header holds)";
          return false;
        }
      }
      list.push_back(g);
    }
    (*dest)[day] = std::move(list);
  }
  return true;
}

// Whole-file read (portable: fopen works on IDF VFS and host stdio alike).
static bool read_file(const std::string &path, std::string *out) {
  FILE *f = fopen(path.c_str(), "rb");
  if (f == nullptr)
    return false;
  out->clear();
  char buf[4096];
  size_t n;
  while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
    out->append(buf, n);
  bool ok = ferror(f) == 0;
  fclose(f);
  return ok;
}

bool settings_filename_ok(const std::string &path, std::string *err) {
  // Path(path).suffix == '.yml' (case sensitive, last extension exactly)
  size_t slash = path.find_last_of("/\\");
  size_t dot = path.find_last_of('.');
  bool ok = dot != std::string::npos && (slash == std::string::npos || dot > slash) &&
            path.compare(dot, std::string::npos, ".yml") == 0 &&
            path.size() - dot == 4;
  if (!ok && err != nullptr)
    *err = "Settings file " + path + " *must* have a .yml suffix";
  return ok;
}

void PiTimerSettings::reset() {
  settings_fn.clear();
  enabled = false;
  font_sizes = {{"header", 0},
                {"timer_4_digits", 0},
                {"timer_5_digits", 0},
                {"pace_of_play", 0},
                {"footer", 0}};
  headers.clear();
  for (const char *attr : {"text", "color"}) {
    std::map<std::string, std::string> evs;
    for (const char *ev : {"when_disabled", "when_idle", "counting_down", "during_buzzer"})
      evs[ev] = "";
    headers[attr] = evs;
  }
  countdown_colors = {{"counting_down", ""}, {"during_buzzer", ""}};
  pace_of_play = false;
  pace_color.clear();
  pace_unit_name.clear();
  pace_unit_duration = 0;
  date_and_time = false;
  game_name = false;
  relays = std::nullopt;
  ring_at_start = std::nullopt;
  countdown_minutes_before = 0;
  countdown_relays.clear();
  relay_duration = 0;
  relay_start_duration = 0;
  relay_countdown_duration = 0;
  relay_end_duration = 0;
  relay_msg_time = 0;
  relays_enabled = true;
  relay_count = 4;
  relay1_function = "None";
  relay2_function = "None";
  relay3_function = "None";
  relay4_function = "None";
  relay1_enabled = true;
  relay2_enabled = true;
  relay3_enabled = true;
  relay4_enabled = true;
  pregame_countdown = 10;
  allow_midnight_wrap = false;
  pregame_font_size = 0;
  pregame_color = "";
  date_font_size = 52;
  date_color = "";
  time_font_size = 52;
  time_color = "";
  footer_color = "";
  background_color = "black";
  pregame_header_text = "Get ready";
  schedules.clear();
  exceptions.clear();
  valid = false;
}

int PiTimerSettings::header_hold(const PiTimerGame &game) const {
  return game.header_hold_override.has_value() ? *game.header_hold_override : relay_msg_time;
}

bool PiTimerSettings::parse_settings_file(const std::string &path, std::string *err) {
  valid = false;  // reset until parsing passed again
  if (!settings_filename_ok(path, err))
    return false;  // message already set
  std::string text;
  if (!read_file(path, &text)) {
    *err = "settings file not found or unreadable: " + path;
    return false;
  }
  return parse_settings_text(text, path, err);
}

bool PiTimerSettings::parse_settings_text(const std::string &text, const std::string &filename,
                                          std::string *err) {
  valid = false;  // reset until parsing passed again
  settings_fn = filename;
  YamlValue raw;
  if (!yaml_parse(text, &raw, err))
    return false;  // YAMLError equivalent
  if (raw.type != YType::Map) {
    *err = filename + " is empty or not a YAML mapping";
    return false;
  }

  const YamlValue *cfg = require_(&raw, "config", YType::Map, "settings file", err);
  if (cfg == nullptr)
    return false;
  const YamlValue *v;

  if ((v = require_(cfg, "enabled", YType::Bool, "main config section", err)) == nullptr)
    return false;
  enabled = v->b;

  // header subsection
  const YamlValue *header = require_(cfg, "header", YType::Map, "main config section", err);
  if (header == nullptr)
    return false;
  if ((v = require_(header, "font_size", YType::Int, "config header subsection", err)) == nullptr)
    return false;
  font_sizes["header"] = static_cast<int>(v->i);
  for (const char *attribute : {"text", "color"}) {
    const YamlValue *values =
        require_(header, attribute, YType::Map, "config header subsection", err);
    if (values == nullptr)
      return false;
    std::string where = std::string("config header ") + attribute;
    for (const char *event :
         {"when_disabled", "when_idle", "counting_down", "during_buzzer"}) {
      const YamlValue *val = require_(values, event, YType::Str, where.c_str(), err);
      if (val == nullptr)
        return false;
      headers[attribute][event] = val->s;
    }
  }

  // countdown subsection
  const YamlValue *countdown = require_(cfg, "countdown", YType::Map, "main config section", err);
  if (countdown == nullptr)
    return false;
  const char *where = "config countdown subsection";
  if ((v = require_(countdown, "font_size_4_digits", YType::Int, where, err)) == nullptr)
    return false;
  font_sizes["timer_4_digits"] = static_cast<int>(v->i);
  if ((v = require_(countdown, "font_size_5_digits", YType::Int, where, err)) == nullptr)
    return false;
  font_sizes["timer_5_digits"] = static_cast<int>(v->i);
  const YamlValue *cd_color = require_(countdown, "color", YType::Map, where, err);
  if (cd_color == nullptr)
    return false;
  for (const char *event : {"counting_down", "during_buzzer"}) {
    const YamlValue *val = require_(cd_color, event, YType::Str, "config countdown color", err);
    if (val == nullptr)
      return false;
    countdown_colors[event] = val->s;
  }

  // pace_of_play subsection
  const YamlValue *pace = require_(cfg, "pace_of_play", YType::Map, "main config section", err);
  if (pace == nullptr)
    return false;
  where = "config pace_of_play subsection";
  if ((v = require_(pace, "enabled", YType::Bool, where, err)) == nullptr)
    return false;
  pace_of_play = v->b;
  if ((v = require_(pace, "font_size", YType::Int, where, err)) == nullptr)
    return false;
  font_sizes["pace_of_play"] = static_cast<int>(v->i);
  if ((v = require_(pace, "unit_duration", YType::Int, where, err)) == nullptr)
    return false;
  pace_unit_duration = static_cast<int>(v->i);
  if (pace_unit_duration <= 0) {
    *err = std::string("'unit_duration' in ") + where + " must be positive";
    return false;
  }
  if ((v = require_(pace, "unit_name", YType::Str, where, err)) == nullptr)
    return false;
  pace_unit_name = v->s;
  if ((v = require_(pace, "color", YType::Str, where, err)) == nullptr)
    return false;
  pace_color = v->s;

  // footer subsection
  const YamlValue *footer = require_(cfg, "footer", YType::Map, "main config section", err);
  if (footer == nullptr)
    return false;
  where = "config footer subsection";
  if ((v = require_(footer, "font_size", YType::Int, where, err)) == nullptr)
    return false;
  font_sizes["footer"] = static_cast<int>(v->i);
  if ((v = require_(footer, "datetime", YType::Bool, where, err)) == nullptr)
    return false;
  date_and_time = v->b;
  if ((v = require_(footer, "entry_name", YType::Bool, where, err)) == nullptr)
    return false;
  game_name = v->b;
  if ((v = footer->find("date_font_size")) != nullptr && v->type == YType::Int) date_font_size = static_cast<int>(v->i);
  if ((v = footer->find("date_color")) != nullptr && v->type == YType::Str) date_color = v->s;
  if ((v = footer->find("time_font_size")) != nullptr && v->type == YType::Int) time_font_size = static_cast<int>(v->i);
  if ((v = footer->find("time_color")) != nullptr && v->type == YType::Str) time_color = v->s;
  if ((v = footer->find("color")) != nullptr && v->type == YType::Str) footer_color = v->s;
  if ((v = cfg->find("relays_enabled")) != nullptr && v->type == YType::Bool) relays_enabled = v->b;
  if ((v = cfg->find("relay_count")) != nullptr && v->type == YType::Int) relay_count = static_cast<int>(v->i);
  for (int i=1;i<=4;i++){ std::string key="relay"+std::to_string(i)+"_function"; if((v=cfg->find(key))!=nullptr && v->type==YType::Str){ if(i==1)relay1_function=v->s; if(i==2)relay2_function=v->s; if(i==3)relay3_function=v->s; if(i==4)relay4_function=v->s;} }
  for (int i=1;i<=4;i++){ std::string key="relay"+std::to_string(i)+"_enabled"; if((v=cfg->find(key))!=nullptr && v->type==YType::Bool){ if(i==1)relay1_enabled=v->b; if(i==2)relay2_enabled=v->b; if(i==3)relay3_enabled=v->b; if(i==4)relay4_enabled=v->b;} }
  if ((v = cfg->find("pregame_countdown")) != nullptr && v->type == YType::Int) pregame_countdown = static_cast<int>(v->i);
  if ((v = cfg->find("allow_midnight_wrap")) != nullptr && v->type == YType::Bool) allow_midnight_wrap = v->b;
  if ((v = cfg->find("pregame_font_size")) != nullptr && v->type == YType::Int) pregame_font_size = static_cast<int>(v->i);
  if ((v = cfg->find("pregame_color")) != nullptr && v->type == YType::Str) pregame_color = v->s;

  if ((v = cfg->find("background_color")) != nullptr && v->type == YType::Str) background_color = v->s;
  if ((v = cfg->find("pregame_header_text")) != nullptr && v->type == YType::Str) pregame_header_text = v->s;

  // buzzer subsection
  const YamlValue *buzzer = require_(cfg, "buzzer", YType::Map, "main config section", err);
  if (buzzer == nullptr)
    return false;
  where = "config buzzer section";
  const YamlValue *rl = require_(buzzer, "relays", YType::Seq, where, err);
  if (rl == nullptr)
    return false;
  relays = std::vector<std::string>{};
  if (!parse_relay_seq(*rl, "relays", where, &*relays, err))
    return false;
  // Optional ring events (absent = feature off; see default_settings.h).
  ring_at_start = std::nullopt;
  if ((v = buzzer->find("ring_at_start")) != nullptr) {
    if (v->type != YType::Seq) {
      *err = std::string("'ring_at_start' in ") + where + " is not a valid list (" +
             yaml_repr(*v) + ")";
      return false;
    }
    ring_at_start = std::vector<std::string>{};
    if (!parse_relay_seq(*v, "ring_at_start", where, &*ring_at_start, err))
      return false;
  }
  countdown_minutes_before = 0;
  countdown_relays.clear();
  if ((v = buzzer->find("countdown")) != nullptr) {
    if (v->type != YType::Map) {
      *err = std::string("'countdown' in ") + where + " is not a valid section (" + yaml_repr(*v) +
             ")";
      return false;
    }
    const char *cd_where = "config buzzer countdown section";
    const YamlValue *minutes = require_(v, "minutes_before", YType::Int, cd_where, err);
    if (minutes == nullptr)
      return false;
    if (minutes->i < 0) {
      *err = "'minutes_before' in config buzzer countdown section cannot be negative";
      return false;
    }
    countdown_minutes_before = static_cast<int>(minutes->i);
    const YamlValue *cd_relays = require_(v, "relays", YType::Seq, cd_where, err);
    if (cd_relays == nullptr)
      return false;
    if (!parse_relay_seq(*cd_relays, "relays", cd_where, &countdown_relays, err))
      return false;
  }
  if ((v = require_(buzzer, "duration", YType::Int, where, err)) == nullptr)
    return false;
  relay_duration = static_cast<int>(v->i);
  if ((v = require_(buzzer, "header_hold", YType::Int, where, err)) == nullptr)
    return false;
  relay_msg_time = static_cast<int>(v->i);
  if ((v = buzzer->find("start_duration")) != nullptr && v->type == YType::Int) relay_start_duration = static_cast<int>(v->i);
  if ((v = buzzer->find("countdown_duration")) != nullptr && v->type == YType::Int) relay_countdown_duration = static_cast<int>(v->i);
  if ((v = buzzer->find("end_duration")) != nullptr && v->type == YType::Int) relay_end_duration = static_cast<int>(v->i);
  if ((v = buzzer->find("relays_enabled")) != nullptr && v->type == YType::Bool) relays_enabled = v->b;
  if ((v = buzzer->find("relay_count")) != nullptr && v->type == YType::Int) relay_count = static_cast<int>(v->i);
  for (int i=1;i<=4;i++){ std::string key="relay"+std::to_string(i)+"_function"; if((v=buzzer->find(key))!=nullptr && v->type==YType::Str){ if(i==1)relay1_function=v->s; if(i==2)relay2_function=v->s; if(i==3)relay3_function=v->s; if(i==4)relay4_function=v->s;} }
  for (int i=1;i<=4;i++){ std::string key="relay"+std::to_string(i)+"_enabled"; if((v=buzzer->find(key))!=nullptr && v->type==YType::Bool){ if(i==1)relay1_enabled=v->b; if(i==2)relay2_enabled=v->b; if(i==3)relay3_enabled=v->b; if(i==4)relay4_enabled=v->b;} }

  const YamlValue *schedule = require_(&raw, "schedule", YType::Map, "settings file", err);
  if (schedule == nullptr)
    return false;
  if (!parse_schedules_into(*schedule, &schedules, relay_msg_time, err))
    return false;
  const YamlValue *exceptions_v = require_(&raw, "exceptions", YType::Map, "settings file", err);
  if (exceptions_v == nullptr)
    return false;
  if (!parse_schedules_into(*exceptions_v, &exceptions, relay_msg_time, err))
    return false;

  // Check that all seven days of week accounted for in schedules
  for (const char *day_of_week : DAY_NAMES) {
    if (schedules.find(day_of_week) == schedules.end()) {
      *err = std::string("missing ") + day_of_week +
             " in schedule, define empty list '[]' as minimum";
      return false;
    }
  }

  valid = true;
  return true;
}

std::optional<PiTimerGame> PiTimerSettings::find_current_game(int weekday,
                                                               int seconds_since_midnight,
                                                               const std::string &date_iso) const {
  if (weekday < 0 || weekday > 6)
    return std::nullopt;
  const std::vector<PiTimerGame> *list = nullptr;
  auto it = schedules.find(DAY_NAMES[weekday]);
  if (it != schedules.end())
    list = &it->second;
  // Check if today is defined as an exception in config file and override
  auto eit = exceptions.find(date_iso);
  if (eit != exceptions.end())
    list = &eit->second;
  if (list == nullptr)
    return std::nullopt;
  // Check if any entry is in progress based on current time; the window takes
  // into account extra buzzer header hold time and returns a negative
  // seconds_until_buzzer while inside that hold time
  for (const PiTimerGame &entry : *list) {
    const int pregame = pregame_countdown;
    if (seconds_since_midnight >= entry.start - pregame &&
        seconds_since_midnight <= entry.buzzer + header_hold(entry)) {
      PiTimerGame copy = entry;
      copy.seconds_until_buzzer = copy.buzzer - seconds_since_midnight;
      copy.seconds_since_start = seconds_since_midnight - copy.start;
      return copy;
    }
  }
  return std::nullopt;
}

}  // namespace curling_engine
}  // namespace esphome
