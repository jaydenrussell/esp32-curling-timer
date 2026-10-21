#pragma once

// Structured Config tab support: apply dotted config values (already
// YAML-encoded scalars) to settings-file text the same way editor.py's
// set_enabled does one key — comments and everything outside the edited
// lines survive byte for byte. Host-tested (test_config_edit.cpp).

#include <string>
#include <vector>

namespace esphome {
namespace curling_engine {

struct ConfigValue {
  std::string path;   // dotted, e.g. "config.header.font_size"
  std::string yaml;   // scalar text, e.g. "208", "True", "'CLUB NAME'", "['RELAY1']"
};

// Return text with each path's scalar replaced (comments on the line kept).
// Fails when a path is missing from the file.
bool apply_config_values(const std::string &text, const std::vector<ConfigValue> &values,
                         std::string *out, std::string *err);

// Encoders for the value shapes the Config tab sends.
std::string yaml_bool(bool value);   // True / False (python str(bool))
std::string yaml_int(long long value);
std::string yaml_quote(const std::string &value);  // 'single quoted', '' escape
std::string yaml_relays(const std::vector<std::string> &relays);  // ['RELAY1', 'RELAY2']

}  // namespace curling_engine
}  // namespace esphome
