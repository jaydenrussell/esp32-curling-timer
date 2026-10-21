#include "web_editor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace web_editor {

static const char *const TAG = "web_editor";

void WebEditorComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Web Editor (WIP stub): port %u", this->port_);
  ESP_LOGCONFIG(TAG, "  Engine: %s", this->engine_ != nullptr ? "configured" : "MISSING");
  // password is never logged
}

}  // namespace web_editor
}  // namespace esphome
