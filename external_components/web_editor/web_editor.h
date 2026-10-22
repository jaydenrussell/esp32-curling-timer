#pragma once

// WIP: HTTP/UI land in the next step (web.py parity API + ported web.html
// with the Config tab). This step adds the auth module + NVS persistence.

#include <string>

#include "auth.h"
#include "esphome/components/curling_engine/curling_engine.h"
#include "esphome/core/component.h"

namespace esphome {
namespace web_editor {

class WebEditorComponent : public Component {
 public:
  void set_engine(curling_engine::CurlingEngineComponent *engine) { engine_ = engine; }
  void set_password(const std::string &password) { password_ = password; }
  void set_port(uint16_t port) { port_ = port; }

  void setup() override;
  void dump_config() override;

  WebConfig &web_config() { return web_config_; }
  LoginLimiter &login_limiter() { return limiter_; }
  curling_engine::CurlingEngineComponent *engine() { return engine_; }
  uint16_t port() const { return port_; }

  // One-line state of the web server for diagnostics (shown as an ESPHome
  // diagnostic entity): "listening on port 8080" once the HTTP server is
  // up, or the reason it is not ("password too short", "httpd_start
  // failed: ..."). Empty until setup() has run.
  std::string status() const { return status_; }

 protected:
  bool load_auth_from_nvs();
  bool save_auth_to_nvs();

  curling_engine::CurlingEngineComponent *engine_{nullptr};
  std::string password_;
  uint16_t port_{8080};
  WebConfig web_config_;
  LoginLimiter limiter_;
  std::string status_;
};

}  // namespace web_editor
}  // namespace esphome
