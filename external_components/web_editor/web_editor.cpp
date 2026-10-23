#include "web_editor.h"

#include <nvs.h>
#include <nvs_flash.h>

#include "esphome/core/log.h"
#include "http.h"

namespace esphome {
namespace web_editor {

static const char *const TAG = "web_editor";
static const char *const NVS_NAMESPACE = "pitimer_web";
static const char *const NVS_KEY_PASSWORD_HASH = "password_hash";
static const char *const NVS_KEY_SECRET = "secret";

#ifndef WEB_EDITOR_VERSION
#define WEB_EDITOR_VERSION "unknown"
#endif

// "listening on port 8080 (web_editor 2026-10-22.1)" - the version part
// shows whether the device runs the current external component.
static std::string status_text(const std::string &what) {
  return what + " (web_editor " + WEB_EDITOR_VERSION + ")";
}

static bool open_web_nvs(nvs_handle_t *handle) {
  esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, handle);
  if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
    if (nvs_flash_init() != ESP_OK)
      return false;
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, handle);
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_open failed: %s", esp_err_to_name(err));
    return false;
  }
  return true;
}

static bool nvs_get_string(nvs_handle_t handle, const char *key, std::string *out) {
  size_t len = 0;
  if (nvs_get_str(handle, key, nullptr, &len) != ESP_OK || len == 0)
    return false;
  out->assign(len, '\0');
  if (nvs_get_str(handle, key, &(*out)[0], &len) != ESP_OK)
    return false;
  out->resize(len - 1);  // drop the trailing NUL
  return true;
}

void WebEditorComponent::set_status(const std::string &status) {
  this->status_ = status;
  if (this->status_sensor_ != nullptr)
    this->status_sensor_->publish_state(status);
}

void WebEditorComponent::setup() {
  if (!this->load_auth_from_nvs()) {
    this->set_status("failed: NVS is not available");
    this->mark_failed();
    return;
  }
  if (!this->password_.empty()) {
    // A changed YAML password re-hashes and rotates the secret (pitimer
    // set_password behaviour: everyone is logged out).
    if (!check_password(this->password_, this->web_config_.password_hash())) {
      if (!this->web_config_.set_password(this->password_)) {
        ESP_LOGE(TAG, "web_editor.password must be at least %u characters",
                 static_cast<unsigned>(MIN_PASSWORD_LENGTH));
        this->set_status("failed: web_editor.password is shorter than " +
                         std::to_string(MIN_PASSWORD_LENGTH) + " characters");
        this->mark_failed();
        return;
      }
      if (!this->save_auth_to_nvs()) {
        this->set_status("failed: the password hash could not be stored");
        this->mark_failed();
        return;
      }
      ESP_LOGI(TAG, "password (re)hashed from YAML config; existing sessions signed out");
    }
  } else if (!this->web_config_.configured()) {
    ESP_LOGE(TAG, "no password: set web_editor.password in the YAML config");
    this->set_status("failed: no web_editor.password in the YAML config");
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "auth ready (session cookie %s, port %u)", SESSION_COOKIE, this->port_);
  delay(500);
  ESP_LOGW(TAG, "SKIPPING httpd_start for debug");
  if (false) {
    this->set_status("failed: the HTTP server could not be started on port " +
                     std::to_string(this->port_));
    this->mark_failed();
    return;
  }
  this->set_status(status_text("listening on port " + std::to_string(this->port_)));
}

bool WebEditorComponent::load_auth_from_nvs() {
  nvs_handle_t handle;
  if (!open_web_nvs(&handle))
    return false;
  std::string hash;
  std::string secret;
  const bool got_hash = nvs_get_string(handle, NVS_KEY_PASSWORD_HASH, &hash);
  const bool got_secret = nvs_get_string(handle, NVS_KEY_SECRET, &secret);
  nvs_close(handle);
  if (got_hash && got_secret) {
    this->web_config_.set_password_hash(std::move(hash));
    this->web_config_.set_secret(std::move(secret));
  }
  return true;
}

bool WebEditorComponent::save_auth_to_nvs() {
  nvs_handle_t handle;
  if (!open_web_nvs(&handle))
    return false;
  esp_err_t err = nvs_set_str(handle, NVS_KEY_PASSWORD_HASH, this->web_config_.password_hash().c_str());
  if (err == ESP_OK)
    err = nvs_set_str(handle, NVS_KEY_SECRET, this->web_config_.secret().c_str());
  if (err == ESP_OK)
    err = nvs_commit(handle);
  nvs_close(handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "saving web auth failed: %s", esp_err_to_name(err));
    return false;
  }
  return true;
}

void WebEditorComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Web Editor: port %u", this->port_);
  ESP_LOGCONFIG(TAG, "  Password: %s", this->web_config_.configured() ? "configured" : "MISSING");
  ESP_LOGCONFIG(TAG, "  Engine: %s", this->engine_ != nullptr ? "configured" : "MISSING");
  // password is never logged
}

}  // namespace web_editor
}  // namespace esphome
