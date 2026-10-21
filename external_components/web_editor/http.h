#pragma once

// esp_http_server glue for the pitimer web API (device only). One wildcard
// handler routes everything into PiTimerApi; the UI page comes with the
// embedded web_ui step.

namespace esphome {
namespace web_editor {

class WebEditorComponent;

// Start the HTTP server on editor->port(). False on failure (caller
// mark_failed()s the component).
bool start_web_httpd(WebEditorComponent *editor);

}  // namespace web_editor
}  // namespace esphome
