#include "http.h"

#include <cstring>
#include <string>

#include "esp_http_server.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

#include "api.h"
#include "auth.h"
#include "esphome/core/log.h"
#include "web_editor.h"
#include "web_ui.h"

namespace esphome {
namespace web_editor {

static const char *const TAG = "web_editor";

// header buffer sizes (larger values truncated to fit)
static constexpr size_t COOKIE_BUF = 1024;
static constexpr size_t SMALL_HEADER_BUF = 256;

static WebEditorComponent *s_editor = nullptr;
static httpd_handle_t s_server = nullptr;

static std::string header_value(httpd_req_t *req, const char *name, size_t buf_size) {
  std::string value;
  value.assign(buf_size, '\0');
  const esp_err_t err = httpd_req_get_hdr_value_str(req, name, &value[0], buf_size);
  if (err != ESP_OK)
    return "";
  value.resize(std::strlen(value.c_str()));
  return value;
}

static std::string client_address(httpd_req_t *req) {
  const int fd = httpd_req_to_sockfd(req);
  if (fd < 0)
    return "";
  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  socklen_t len = sizeof(addr);
  if (getpeername(fd, reinterpret_cast<struct sockaddr *>(&addr), &len) != 0)
    return "";
  if (addr.sin_family != AF_INET)
    return "";
  char buf[INET_ADDRSTRLEN];
  std::memset(buf, 0, sizeof(buf));
  inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
  return buf;
}

static bool read_body(httpd_req_t *req, ApiRequest *request) {
  const size_t length = req->content_len;
  if (length > MAX_BODY_BYTES) {
    request->body_too_large = true;
    return true;  // respond 413 without buffering
  }
  if (length == 0)
    return true;  // empty body — python treats as {}
  request->body.resize(length);
  size_t received = 0;
  while (received < length) {
    const int got = httpd_req_recv(req, &request->body[received], length - received);
    if (got <= 0) {
      if (got == HTTPD_SOCK_ERR_TIMEOUT)
        continue;
      ESP_LOGW(TAG, "body recv failed");
      return false;
    }
    received += static_cast<size_t>(got);
  }
  return true;
}

static const char *status_line(int status) {
  switch (status) {
    case 200:
      return "200 OK";
    case 400:
      return "400 Bad Request";
    case 401:
      return "401 Unauthorized";
    case 403:
      return "403 Forbidden";
    case 404:
      return "404 Not Found";
    case 405:
      return "405 Method Not Allowed";
    case 409:
      return "409 Conflict";
    case 413:
      return "413 Payload Too Large";
    case 415:
      return "415 Unsupported Media Type";
    case 422:
      return "422 Unprocessable Content";
    case 429:
      return "429 Too Many Requests";
    case 500:
      return "500 Internal Server Error";
    case 501:
      return "501 Not Implemented";
    case 503:
      return "503 Service Unavailable";
    default:
      return "500 Internal Server Error";
  }
}

static esp_err_t send_response(httpd_req_t *req, const ApiResponse &resp) {
  httpd_resp_set_status(req, status_line(resp.status));
  httpd_resp_set_type(req, resp.content_type.c_str());
  // python RequestHandler._send security headers
  httpd_resp_set_hdr(req, "Cache-Control", "no-store");
  httpd_resp_set_hdr(req, "X-Content-Type-Options", "nosniff");
  httpd_resp_set_hdr(req, "X-Frame-Options", "DENY");
  httpd_resp_set_hdr(req, "Referrer-Policy", "no-referrer");
  for (const auto &header : resp.extra_headers)
    httpd_resp_set_hdr(req, header.first.c_str(), header.second.c_str());
  if (resp.body.empty())
    return httpd_resp_send(req, nullptr, 0);
  return httpd_resp_send(req, resp.body.data(), static_cast<ssize_t>(resp.body.size()));
}

static esp_err_t api_handler(httpd_req_t *req) {
  ApiRequest request;
  if (req->method == HTTP_GET)
    request.method = "GET";
  else if (req->method == HTTP_POST)
    request.method = "POST";
  else
    request.method = "?";
  // strip the query string (python urlsplit().path)
  std::string uri = req->uri;
  const size_t query = uri.find('?');
  if (query != std::string::npos)
    uri.resize(query);
  request.path = std::move(uri);
  // the editor page itself (python serves web.html on /); CSP as in web.py _send
  if (request.method == "GET" && (request.path == "/" || request.path == "/index.html")) {
    ApiResponse page;
    page.content_type = "text/html; charset=utf-8";
    page.extra_headers.emplace_back(
        "Content-Security-Policy",
        "default-src 'self'; script-src 'self' 'unsafe-inline'; "
        "style-src 'self' 'unsafe-inline'; frame-ancestors 'none'");
    page.body = WEB_UI_HTML;
    return send_response(req, page);
  }
  request.content_type = header_value(req, "Content-Type", SMALL_HEADER_BUF);
  request.origin = header_value(req, "Origin", SMALL_HEADER_BUF);
  request.host = header_value(req, "Host", SMALL_HEADER_BUF);
  request.cookie = header_value(req, "Cookie", COOKIE_BUF);
  request.client_address = client_address(req);
  if (request.method == "POST" && !read_body(req, &request))
    return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "body read failed");
  if (s_editor == nullptr || s_editor->engine() == nullptr) {
    ApiResponse unavailable;
    unavailable.status = 503;
    unavailable.body = "{\"error\":\"web editor not ready\"}";
    return send_response(req, unavailable);
  }
  PiTimerApi api(s_editor->engine(), &s_editor->web_config(), &s_editor->login_limiter());
  return send_response(req, api.handle(request));
}

bool start_web_httpd(WebEditorComponent *editor) {
  s_editor = editor;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = editor->port();
  config.ctrl_port = editor->port() + 1;
  config.max_uri_handlers = 4;
  // A single user (the web editor) needs one or two connections; keeping
  // the defaults costs internal RAM on a device that also runs LVGL, the
  // HDMI driver and the API.
  config.max_open_sockets = 4;
  config.stack_size = 8192;
  // The LRU purge path takes the httpd's control-socket semaphore; with a
  // freed server that handle can be NULL and the take aborts the chip.
  // Without it the server just refuses extra connections.
  config.lru_purge_enable = false;
  config.uri_match_fn = httpd_uri_match_wildcard;
  ESP_LOGI(TAG, "free heap before the web server: %u bytes (largest block %u)",
           static_cast<unsigned>(esp_get_free_heap_size()),
           static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)));
  esp_err_t err = httpd_start(&s_server, &config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "httpd_start on port %u failed: %s (free heap %u bytes)", editor->port(),
             esp_err_to_name(err), static_cast<unsigned>(esp_get_free_heap_size()));
    s_server = nullptr;
    return false;
  }
  httpd_uri_t api_get = {};
  api_get.uri = "/*";
  api_get.method = HTTP_GET;
  api_get.handler = api_handler;
  httpd_uri_t api_post = {};
  api_post.uri = "/*";
  api_post.method = HTTP_POST;
  api_post.handler = api_handler;
  err = httpd_register_uri_handler(s_server, &api_get);
  if (err == ESP_OK)
    err = httpd_register_uri_handler(s_server, &api_post);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "registering API handlers failed: %s", esp_err_to_name(err));
    httpd_stop(s_server);
    s_server = nullptr;
    return false;
  }
  ESP_LOGI(TAG, "web API listening on port %u", editor->port());
  return true;
}

}  // namespace web_editor
}  // namespace esphome
