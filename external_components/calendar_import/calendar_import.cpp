#include "calendar_import.h"

#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

#include <esp_http_client.h>
#include <sstream>

namespace esphome {
namespace calendar_import {

static const char *const TAG = "calendar_import";

void CalendarImport::setup() {}

void CalendarImport::dump_config() {
  ESP_LOGCONFIG(TAG, "Calendar Import:");
  ESP_LOGCONFIG(TAG, "  URL: %s", url_.c_str());
}

std::string CalendarImport::fetch_ics_() {
  std::string body;
  esp_http_client_config_t config = {};
  config.url = url_.c_str();
  config.timeout_ms = 15000;
  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (!client) return body;
  esp_http_client_set_method(client, HTTP_METHOD_GET);
  if (esp_http_client_open(client, 0) == ESP_OK) {
    int content_length = esp_http_client_fetch_headers(client);
    char buffer[1024];
    int read_len;
    if (content_length > 0) {
      while ((read_len = esp_http_client_read(client, buffer, sizeof(buffer))) > 0) {
        body.append(buffer, read_len);
      }
    } else {
      while ((read_len = esp_http_client_read(client, buffer, sizeof(buffer))) > 0) {
        body.append(buffer, read_len);
      }
    }
  }
  esp_http_client_close(client);
  esp_http_client_cleanup(client);
  return body;
}

std::string CalendarImport::get_field_(const std::string &block, const std::string &key) {
  size_t pos = 0;
  while ((pos = block.find(key, pos)) != std::string::npos) {
    if (pos == 0 || block[pos - 1] == '\n' || block[pos - 1] == '\r') {
      size_t line_end = block.find('\n', pos);
      if (line_end == std::string::npos) line_end = block.size();
      std::string line = block.substr(pos, line_end - pos);
      // strip key and optional params up to colon
      size_t colon = line.find(':');
      if (colon != std::string::npos) {
        return unescape_(line.substr(colon + 1));
      }
    }
    pos++;
  }
  return "";
}

std::string CalendarImport::unescape_(const std::string &v) {
  std::string out;
  for (size_t i = 0; i < v.size(); i++) {
    if (v[i] == '\\' && i + 1 < v.size()) {
      char c = v[i + 1];
      if (c == 'n' || c == 'N') { out += '\n'; i++; continue; }
      if (c == ',' || c == ';' || c == '\\') { out += c; i++; continue; }
    }
    out += v[i];
  }
  // trim trailing CR
  while (!out.empty() && (out.back() == '\r' || out.back() == ' ')) out.pop_back();
  return out;
}

std::time_t CalendarImport::parse_dt_(const std::string &value) {
  // handle forms: 20230101T120000Z, 20230101T120000, 20230101
  std::string v = value;
  // remove any tzid prefix already stripped
  std::tm tm = {};
  if (v.size() >= 15 && v[8] == 'T') {
    // YYYYMMDDTHHMMSS
    tm.tm_year = std::stoi(v.substr(0, 4)) - 1900;
    tm.tm_mon = std::stoi(v.substr(4, 2)) - 1;
    tm.tm_mday = std::stoi(v.substr(6, 2));
    tm.tm_hour = std::stoi(v.substr(9, 2));
    tm.tm_min = std::stoi(v.substr(11, 2));
    tm.tm_sec = std::stoi(v.substr(13, 2));
  } else if (v.size() >= 8) {
    tm.tm_year = std::stoi(v.substr(0, 4)) - 1900;
    tm.tm_mon = std::stoi(v.substr(4, 2)) - 1;
    tm.tm_mday = std::stoi(v.substr(6, 2));
  }
  std::time_t t = std::mktime(&tm);
  if (!v.empty() && v.back() == 'Z') {
    // treat as UTC: adjust by local offset roughly not exact; leave as mktime UTC-ish
  }
  return t;
}

std::vector<CalendarEvent> CalendarImport::parse_ics_(const std::string &ics) {
  std::vector<CalendarEvent> events;
  size_t pos = 0;
  while (true) {
    size_t start = ics.find("BEGIN:VEVENT", pos);
    if (start == std::string::npos) break;
    size_t end = ics.find("END:VEVENT", start);
    if (end == std::string::npos) break;
    std::string block = ics.substr(start, end - start);
    CalendarEvent ev;
    std::string dtstart = get_field_(block, "DTSTART");
    std::string dtend = get_field_(block, "DTEND");
    // handle TZID params: field key may be DTSTART;TZID=... but our get_field searches "DTSTART" prefix ok
    ev.start = parse_dt_(dtstart);
    ev.end = parse_dt_(dtend);
    ev.summary = get_field_(block, "SUMMARY");
    ev.description = get_field_(block, "DESCRIPTION");
    ev.location = get_field_(block, "LOCATION");
    // extract sheet/ice number from location or summary
    // team names: if summary like "Team A vs Team B"
    size_t vspos = ev.summary.find(" vs ");
    if (vspos != std::string::npos) {
      ev.team_a = ev.summary.substr(0, vspos);
      ev.team_b = ev.summary.substr(vspos + 4);
    } else {
      ev.team_a = ev.summary;
    }
    // sheet number from location digits
    for (char c : ev.location) {
      if (isdigit((unsigned char)c)) { ev.sheet = c - '0'; break; }
    }
    events.push_back(ev);
    pos = end + 10;
  }
  return events;
}

std::vector<CalendarEvent> CalendarImport::fetch_and_parse() {
  std::string ics = fetch_ics_();
  return parse_ics_(ics);
}

}  // namespace calendar_import
}  // namespace esphome
