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

#include <cstdlib>
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

// Every quoted option value in the display-test-mode <select>.
std::vector<std::string> dropdown_modes(const std::string &html) {
  const std::string anchor = "api('/api/display_test_mode'";
  const size_t at = html.find(anchor);
  REQUIRE(at != std::string::npos);
  // The options follow the onchange handler; stop at the next card.
  const size_t stop = html.find("const section1", at);
  REQUIRE(stop != std::string::npos);
  std::vector<std::string> out;
  const std::string chunk = html.substr(at, stop - at);
  const std::string opt = "el('option', { value: '";
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

// True when `needle` appears inside the block that starts at `start`.
bool contains_between(const std::string &hay, size_t start, const std::string &needle) {
  const size_t at = hay.find(needle, start);
  if (at == std::string::npos)
    return false;
  const size_t stop = hay.find("\n    - ", at);
  return stop == std::string::npos || at < stop;
}

// Every "      - lvgl.widget.update:" block in the interval.
std::vector<std::string> update_blocks(const std::string &yaml) {
  std::vector<std::string> out;
  const std::string anchor = "      - lvgl.widget.update:";
  size_t pos = 0;
  while ((pos = yaml.find(anchor, pos)) != std::string::npos) {
    const size_t stop = yaml.find("\n      - ", pos + 1);
    out.push_back(yaml.substr(pos, stop == std::string::npos ? std::string::npos : stop - pos));
    pos = stop == std::string::npos ? yaml.size() : stop;
  }
  return out;
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
    bool found = false;
    for (const std::string &v : valid)
      if (v == mode)
        found = true;
    CHECK(found);
  }
}

TEST_CASE("display test mode: every non-normal mode has a hidden pattern group") {
  const std::string html = read_file(repo_file("external_components/web_editor/web_ui.html"));
  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  REQUIRE(!html.empty());
  REQUIRE(!yaml.empty());

  const std::vector<std::string> options = dropdown_modes(html);
  REQUIRE(!options.empty());

  for (const std::string &mode : options) {
    if (mode == "normal")
      continue;
    INFO("mode " << mode);

    // smpte and colorbars share one group of bars.
    const std::string group = mode == "colorbars" ? "test_smpte" : "test_" + mode;

    const size_t cid = yaml.find("id: " + group);
    REQUIRE(cid != std::string::npos);

    // The group must start hidden so nothing shows at boot.
    const size_t block_end = yaml.find("\n    - ", cid + 1);
    REQUIRE(block_end != std::string::npos);
    CHECK(yaml.substr(cid, block_end - cid).find("hidden: true") != std::string::npos);

    // Exactly one interval toggle must drive that group, and it must key off
    // the selected mode.
    const std::vector<std::string> updates = update_blocks(yaml);
    std::vector<std::string> mine;
    for (const std::string &u : updates)
      if (u.find("id: " + group) != std::string::npos)
        mine.push_back(u);
    REQUIRE(mine.size() == 1);
    CHECK(mine[0].find("get_display_test_mode()") != std::string::npos);
    CHECK(mine[0].find("\"" + mode + "\"") != std::string::npos);
  }
}

TEST_CASE("display test mode: normal hides every pattern group") {
  const std::string yaml = read_file(repo_file("esphome/curling-timer.yaml"));
  REQUIRE(!yaml.empty());

  // Every group toggle must be driven by the mode, so "normal" leaves them
  // hidden (the lambda only unhides on a matching non-normal mode).
  const std::vector<std::string> groups = {"test_smpte", "test_pluge", "test_grayscale", "test_geometry"};
  for (const std::string &group : groups) {
    INFO("group " << group);
    const size_t upd = yaml.find("id: " + group);
    REQUIRE(upd != std::string::npos);
    CHECK(contains_between(yaml, upd, "get_display_test_mode()"));
  }
}
