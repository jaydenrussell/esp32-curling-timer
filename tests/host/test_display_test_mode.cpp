// Contract test for the Diagnostics -> Display Test Mode feature.
//
// The bug this guards against: the web UI's api() helper takes the JSON
// payload as its SECOND argument and stringifies it itself
//     body === undefined ? {} : { method:'POST', ..., body: JSON.stringify(body) }
// so calling
//     api('/api/display_test_mode', { method:'POST', body: JSON.stringify({mode}) })
// put {"method":"POST","body":"{\"mode\":\"grayscale\"}"} on the wire. The
// server reads data["mode"], which was then always undefined, so it replied
// 400 "'mode' is required" and the dropdown silently did nothing.
//
// These checks read the real files, so a regression in the browser code, the
// HTTP handler or the ESPHome config fails the build instead of shipping.
#include "doctest.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string read_file(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    return {};
  std::ostringstream buf;
  buf << in.rdbuf();
  return buf.str();
}

// The test binary runs from tests/host/build; walk up to the repo root.
std::string repo_root() {
  std::string dir = ".";
  for (int i = 0; i < 8; i++) {
    if (!read_file(dir + "/esphome/curling-timer.yaml").empty())
      return dir;
    dir += "/..";
  }
  return {};
}

std::string repo_file(const std::string &rel) { return repo_root() + "/" + rel; }

// Return true when the LVGL display section is actually enabled in the YAML
// (i.e. not commented out). Tests that depend on LVGL widgets are skipped
// when the section is still a WIP comment block.
bool lvgl_enabled() {
  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  return yaml.find("\nlvgl:") != std::string::npos;
}

// Every quoted option value in the display-test-mode <select>.
std::vector<std::string> dropdown_modes(const std::string &html) {
  const std::string anchor = "api('/api/display_test_mode'";
  const size_t at = html.find(anchor);
  REQUIRE(at != std::string::npos);
  const size_t stop = html.find("const section1", at);
  REQUIRE(stop != std::string::npos);
  const std::string chunk = html.substr(at, stop - at);
  const std::string opt = "el('option', { value: '";
  std::vector<std::string> out;
  size_t pos = 0;
  while ((pos = chunk.find(opt, pos)) != std::string::npos) {
    pos += opt.size();
    const size_t end = chunk.find('\'', pos);
    REQUIRE(end != std::string::npos);
    out.push_back(chunk.substr(pos, end - pos));
    pos = end;
  }
  return out;
}

// The server-side whitelist in PiTimerApi::display_test_mode.
std::vector<std::string> server_modes(const std::string &api) {
  const std::string anchor = "const char *valid[] = {";
  const size_t at = api.find(anchor);
  REQUIRE(at != std::string::npos);
  const size_t end = api.find("};", at);
  REQUIRE(end != std::string::npos);
  const std::string chunk = api.substr(at + anchor.size(), end - at);
  std::vector<std::string> out;
  size_t pos = 0;
  while ((pos = chunk.find('"', pos)) != std::string::npos) {
    const size_t close = chunk.find('"', pos + 1);
    REQUIRE(close != std::string::npos);
    out.push_back(chunk.substr(pos + 1, close - pos - 1));
    pos = close + 1;
  }
  return out;
}

// One entry per LVGL widget in the lvgl: widgets list.
std::vector<std::string> widget_blocks(const std::string &yaml) {
  std::vector<std::string> out;
  size_t pos = yaml.find("\n  widgets:");
  if (pos == std::string::npos) pos = yaml.find("\r\n  widgets:");
  REQUIRE(pos != std::string::npos);
  pos = yaml.find("\n", pos + 1) + 1;
  size_t stop = yaml.find("\ninterval:", pos);
  if (stop == std::string::npos) stop = yaml.find("\r\ninterval:", pos);
  REQUIRE(stop != std::string::npos);
  const std::string chunk = yaml.substr(pos, stop - pos);
  size_t cur = 0;
  while (true) {
    size_t next = chunk.find("\n    - ", cur + 1);
    if (next == std::string::npos) next = chunk.find("\r\n    - ", cur + 1);
    if (next == std::string::npos) {
      out.push_back(chunk.substr(cur));
      break;
    }
    out.push_back(chunk.substr(cur, next - cur));
    cur = next + 1;
  }
  return out;
}

// The id of a widget block, or "" when it has none.
std::string block_id(const std::string &block) {
  size_t at = block.find("\n        id: ");
  if (at == std::string::npos)
    at = block.find("\r\n        id: ");
  if (at == std::string::npos)
    return {};
  const size_t prefix_len = (at + 1 < block.size() && block[at] == '\r') ? 14 : 13;
  const size_t start = at + prefix_len;
  const size_t end = block.find('\n', start);
  std::string r = block.substr(start, (end == std::string::npos ? block.size() : end) - start);
  if (!r.empty() && r.back() == '\r') r.pop_back();
  return r;
}

// One entry per "      - lvgl.widget.update:" action in the interval.
std::vector<std::string> update_blocks(const std::string &yaml) {
  std::vector<std::string> out;
  const std::string anchor = "      - lvgl.widget.update:";
  size_t pos = 0;
  while ((pos = yaml.find(anchor, pos)) != std::string::npos) {
    size_t stop = yaml.find("\n      - ", pos + 1);
    if (stop == std::string::npos) stop = yaml.find("\r\n      - ", pos + 1);
    out.push_back(yaml.substr(pos, stop == std::string::npos ? std::string::npos : stop - pos));
    pos = stop == std::string::npos ? yaml.size() : stop;
  }
  return out;
}

// The widget ids listed by an update action.
std::vector<std::string> update_ids(const std::string &block) {
  size_t at = block.find("\n          id:\n");
  if (at == std::string::npos) at = block.find("\r\n          id:\r\n");
  if (at == std::string::npos) at = block.find("\n          id:\r\n");
  if (at == std::string::npos) at = block.find("\r\n          id:\n");
  if (at == std::string::npos)
    return {};
  const size_t id_at_len = (at + 1 < block.size() && block[at] == '\r') ? 17 : 15;
  const std::string item = "            - ";
  std::vector<std::string> out;
  size_t pos = at + id_at_len;
  while (pos < block.size() && block.compare(pos, item.size(), item) == 0) {
    const size_t end = block.find('\n', pos);
    if (end == std::string::npos)
      break;
    std::string id = block.substr(pos + item.size(), end - pos - item.size());
    if (!id.empty() && id.back() == '\r') id.pop_back();
    out.push_back(id);
    pos = end + 1;
  }
  return out;
}

bool contains(const std::vector<std::string> &hay, const std::string &needle) {
  for (const std::string &h : hay)
    if (h == needle)
      return true;
  return false;
}

}  // namespace

TEST_CASE("display test mode: the web UI sends the mode as the api() payload") {
  const std::string html = read_file(repo_file("external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());

  const std::string anchor = "api('/api/display_test_mode'";
  const size_t at = html.find(anchor);
  REQUIRE(at != std::string::npos);

  // The window runs from the api() call to the end of the <select> block.
  const size_t stop = html.find("const section1", at);
  REQUIRE(stop != std::string::npos);
  const std::string call = html.substr(at, stop - at);

  // The payload must be { mode: ... } - api() does the POST + JSON encoding.
  CHECK(call.find("{ mode:") != std::string::npos);
  // It must NOT re-wrap the payload in a method/body envelope.
  CHECK(call.find("method:") == std::string::npos);
  CHECK(call.find("JSON.stringify") == std::string::npos);
}

TEST_CASE("display test mode: the server accepts every mode the dropdown offers") {
  const std::string html = read_file(repo_file("external_components/web_editor/web_ui.html"));
  const std::string api = read_file(repo_file("external_components/web_editor/api.cpp"));
  REQUIRE(!html.empty());
  REQUIRE(!api.empty());

  const std::vector<std::string> options = dropdown_modes(html);
  const std::vector<std::string> valid = server_modes(api);
  REQUIRE(!options.empty());
  REQUIRE(!valid.empty());

  for (const std::string &mode : options) {
    INFO("dropdown mode " << mode);
    CHECK(contains(valid, mode));
  }
}

TEST_CASE("display test mode: the lvgl widget list holds nothing unexpected") {
  if (!lvgl_enabled())
    return;

  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  REQUIRE(!yaml.empty());

  // The widget list may contain only the normal display labels and the test
  // pattern widgets. An anonymous widget (no id) is always a bug: it can
  // never be toggled, so it is painted over the display forever. That is
  // exactly how an orphaned copy of the colour bars survived in the list and
  // showed up at boot no matter what the dropdown did.
  const std::vector<std::string> normal_labels = {"lbl_header", "lbl_cnt4", "lbl_cnt5", "lbl_pace",
                                                  "lbl_status", "lbl_game", "lbl_date", "lbl_time"};
  size_t patterns = 0;
  for (const std::string &block : widget_blocks(yaml)) {
    const std::string id = block_id(block);
    if (id.empty()) {
      INFO("anonymous widget block:\n" << block.substr(0, 120));
      CHECK(false);
      continue;
    }
    if (id.rfind("tp_", 0) == 0) {
      patterns++;
      continue;
    }
    INFO("unexpected widget id " << id);
    CHECK(contains(normal_labels, id));
  }
  CHECK(patterns > 0);
}

TEST_CASE("display test mode: every pattern widget is hidden at creation") {
  if (!lvgl_enabled())
    return;

  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  REQUIRE(!yaml.empty());

  size_t found = 0;
  for (const std::string &block : widget_blocks(yaml)) {
    const std::string id = block_id(block);
    if (id.rfind("tp_", 0) != 0)
      continue;
    found++;
    INFO("pattern widget " << id);
    // Nothing may be visible at boot: every pattern widget starts hidden and
    // is only revealed by the matching mode.
    CHECK(block.find("hidden: true") != std::string::npos);
  }
  CHECK(found > 0);
}

TEST_CASE("display test mode: every pattern widget is toggled exactly once") {
  if (!lvgl_enabled())
    return;

  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  const std::string html = read_file(repo_file("external_components/web_editor/web_ui.html"));
  REQUIRE(!yaml.empty());
  REQUIRE(!html.empty());

  std::vector<std::string> patterns;
  for (const std::string &block : widget_blocks(yaml)) {
    const std::string id = block_id(block);
    if (id.rfind("tp_", 0) == 0)
      patterns.push_back(id);
  }
  REQUIRE(!patterns.empty());

  const std::vector<std::string> updates = update_blocks(yaml);
  REQUIRE(!updates.empty());

  for (const std::string &id : patterns) {
    INFO("pattern widget " << id);
    size_t hits = 0;
    for (const std::string &u : updates)
      if (contains(update_ids(u), id))
        hits++;
    CHECK(hits == 1);
  }
}

TEST_CASE("display test mode: the normal output is blanked while a pattern is up") {
  if (!lvgl_enabled())
    return;

  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  REQUIRE(!yaml.empty());

  // Every normal label must render nothing when a test pattern is selected,
  // so the pattern is the only thing on screen.
  const std::vector<std::string> labels = {"lbl_header", "lbl_cnt4", "lbl_cnt5", "lbl_pace",
                                           "lbl_status", "lbl_game", "lbl_date", "lbl_time"};
  const std::string guard = "if (mode != \"normal\") return std::string();";
  for (const std::string &label : labels) {
    INFO("label " << label);
    const size_t at = yaml.find("id: " + label);
    REQUIRE(at != std::string::npos);
    const size_t stop = yaml.find("hidden: true", at);
    const size_t limit = stop == std::string::npos ? yaml.size() : stop;
    const size_t guard_at = yaml.find(guard, at);
    REQUIRE(guard_at != std::string::npos);
    CHECK(guard_at < limit);
  }
}

TEST_CASE("display test mode: each pattern group keys off a real dropdown mode") {
  if (!lvgl_enabled())
    return;

  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  const std::string html = read_file(repo_file("external_components/web_editor/web_ui.html"));
  REQUIRE(!yaml.empty());
  REQUIRE(!html.empty());

  const std::vector<std::string> options = dropdown_modes(html);
  const std::vector<std::string> updates = update_blocks(yaml);
  REQUIRE(!options.empty());
  REQUIRE(!updates.empty());

  // Every non-normal mode in the dropdown must be named by some toggle, so
  // picking it can never be a no-op.
  for (const std::string &mode : options) {
    if (mode == "normal")
      continue;
    INFO("mode " << mode);
    bool found = false;
    for (const std::string &u : updates) {
      if (u.find("get_display_test_mode()") == std::string::npos)
        continue;
      if (u.find("\"" + mode + "\"") != std::string::npos)
        found = true;
    }
    CHECK(found);
  }

  // "normal" must never unhide a pattern: no toggle may name it.
  for (const std::string &u : updates) {
    if (u.find("get_display_test_mode()") == std::string::npos)
      continue;
    CHECK(u.find("\"normal\"") == std::string::npos);
  }
}
