#pragma once

// WIP stub — Phase C (web_editor API + httpd + UI) replaces this with the
// full esp_http_server implementation. Exists now so codegen references
// (set_engine/set_password/set_port) compile and the harness validates.

#include <string>

#include "esphome/components/curling_engine/curling_engine.h"
#include "esphome/core/component.h"

namespace esphome {
namespace web_editor {

class WebEditorComponent : public Component {
 public:
  void set_engine(curling_engine::CurlingEngineComponent *engine) { engine_ = engine; }
  void set_password(const std::string &password) { password_ = password; }
  void set_port(uint16_t port) { port_ = port; }

  void dump_config() override;

 protected:
  curling_engine::CurlingEngineComponent *engine_{nullptr};
  std::string password_;
  uint16_t port_{8080};
};

}  // namespace web_editor
}  // namespace esphome
