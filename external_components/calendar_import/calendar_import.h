#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"

#include <string>
#include <vector>
#include <ctime>

namespace esphome {
namespace calendar_import {

struct CalendarEvent {
  std::time_t start{0};
  std::time_t end{0};
  std::string summary;
  std::string description;
  std::string location;
  std::string team_a;
  std::string team_b;
  int sheet{0};
};

class CalendarImport : public Component {
 public:
  void setup() override;
  void dump_config() override;

  void set_url(const std::string &url) { url_ = url; }

  std::vector<CalendarEvent> fetch_and_parse();

 protected:
  std::string url_;
  std::string fetch_ics_();
  std::vector<CalendarEvent> parse_ics_(const std::string &ics);
  static std::time_t parse_dt_(const std::string &value);
  static std::string unescape_(const std::string &v);
  static std::string get_field_(const std::string &block, const std::string &key);
};

}  // namespace calendar_import
}  // namespace esphome
