#pragma once

// Port of pitimer v2.4.4 web.py PiTimerWeb + RequestHandler routing: the API
// behind the web page, independent of HTTP plumbing so it can be swapped or
// extended. Paths, status codes and error strings match python; GET/POST
// /api/config are new (structured config edit over config_edit splice).
//
// Clock gate (no python parity — python always has a wall clock): until the
// RTC is sane, login / quick-start / cancel answer 503.

#include <string>
#include <utility>
#include <vector>

#include "auth.h"
#include "esphome/components/curling_engine/curling_engine.h"
#include "esphome/components/curling_engine/model.h"
#include "esphome/components/json/json_util.h"

namespace esphome {
namespace web_editor {

// Parsed request pieces the http layer already has (python _read_json checks).
struct ApiRequest {
  std::string method;  // GET or POST
  std::string path;    // URL path, no query
  std::string body;    // raw body bytes (empty when none)
  std::string content_type;
  std::string origin;
  std::string host;
  std::string cookie;
  std::string client_address;
  bool bad_content_length{false};
  bool body_too_large{false};
};

struct ApiResponse {
  int status{200};
  std::string content_type{"application/json"};
  std::string body;
  std::vector<std::pair<std::string, std::string>> extra_headers;
};

class PiTimerApi {
 public:
  PiTimerApi(curling_engine::CurlingEngineComponent *engine, WebConfig *config, LoginLimiter *limiter)
      : engine_(engine), config_(config), limiter_(limiter) {}

  // Route one request; always returns a response ready to send.
  ApiResponse handle(const ApiRequest &req);

 protected:
  struct ParsedLive {
    std::string version;
    bool valid{false};
    bool model_ok{false};
    std::string error;  // first line, like python str(err).splitlines()[0]
    curling_engine::PiTimerSettings settings;
    curling_engine::ScheduleModel model;
  };

  bool parsed_live(ParsedLive *out);
  // 422 helper mirroring python _valid_settings; false + *resp when invalid.
  bool valid_settings(ParsedLive *live, ApiResponse *resp);
  bool logged_in(const std::string &cookie) const;
  bool now_ok(int *weekday_mon0, int *seconds_of_day, std::string *date_iso) const;

  ApiResponse status();
  ApiResponse get_settings();
  ApiResponse put_settings(JsonObjectConst data);
  ApiResponse set_enabled(JsonObjectConst data);
  ApiResponse get_raw();
  ApiResponse put_raw(JsonObjectConst data);
  ApiResponse validate_raw(JsonObjectConst data);
  ApiResponse get_backups();
  ApiResponse get_backup(const std::string &name);
  ApiResponse restore(JsonObjectConst data);
  ApiResponse quick_start(JsonObjectConst data);
  ApiResponse cancel_quick_start(JsonObjectConst data);
  ApiResponse get_config();
  ApiResponse post_config(JsonObjectConst data);
  ApiResponse login(const ApiRequest &req, JsonObjectConst data);
  ApiResponse logout();

  curling_engine::CurlingEngineComponent *engine_;
  WebConfig *config_;
  LoginLimiter *limiter_;
};

}  // namespace web_editor
}  // namespace esphome
