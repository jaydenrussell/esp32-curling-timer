// Regression guard for the browser mirror's countdown colon.
//
// The engine emits U+02F8 (raised colon) for HDMI. Browser monospace fonts do
// not normally include that codepoint, so feeding it straight into a WebUI
// span renders a placeholder box. The mirror must normalize it to ASCII ':'
// and toggle colon opacity when pregame blink is enabled (simple flash).

#include "doctest.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

namespace {

std::string read_file(const std::string &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in)
    return {};
  std::ostringstream buf;
  buf << in.rdbuf();
  return buf.str();
}

std::string strip_cr(std::string value) {
  value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
  return value;
}

std::string replace_all(std::string value, const std::string &from, const std::string &to) {
  if (from.empty())
    return value;
  size_t pos = 0;
  while ((pos = value.find(from, pos)) != std::string::npos) {
    value.replace(pos, from.size(), to);
    pos += to.size();
  }
  return value;
}

std::string repo_root() {
  std::string dir = ".";
  for (int i = 0; i < 8; i++) {
    if (!read_file(dir + "/esphome/curling-timer.yaml").empty())
      return dir;
    dir += "/..";
  }
  return {};
}

}  // namespace

TEST_CASE("web mirror: countdown colons use a browser-safe glyph") {
  std::string html = read_file(repo_root() + "/external_components/web_editor/web_ui.html");
  REQUIRE(!html.empty());
  html = strip_cr(html);

  // Every browser mirror path must map U+02F8 to ASCII ':'.
  const size_t helper = html.find("function browserCountdown(");
  REQUIRE(helper != std::string::npos);
  const size_t helper_end = html.find("\n}\n", helper);
  REQUIRE(helper_end != std::string::npos);
  const std::string normalize = html.substr(helper, helper_end - helper);
  CHECK(normalize.find("ch === '\\u02F8'") != std::string::npos);
  CHECK(normalize.find("? ':' : ch") != std::string::npos);

  const size_t build = html.find("function buildCountdownContent(");
  REQUIRE(build != std::string::npos);
  const size_t build_end = html.find("\n}\n", build);
  REQUIRE(build_end != std::string::npos);
  const std::string builder = html.substr(build, build_end - build);
  CHECK(builder.find("Array.from(browserCountdown(text))") != std::string::npos);

  // No raw engine string may be written straight into the browser mirror.
  CHECK(html.find("mc.textContent = s.countdown;") == std::string::npos);
  CHECK(html.find("}, sm.countdown || '');") == std::string::npos);
  CHECK(html.find("browserCountdown(sm.countdown)") != std::string::npos);
  CHECK(html.find("browserCountdown(s.countdown)") != std::string::npos);

  // The live current-game formatter must also stay browser-safe.
  const size_t fmt = html.find("function countdownText(");
  REQUIRE(fmt != std::string::npos);
  const size_t fmt_end = html.find("\n}\n", fmt);
  REQUIRE(fmt_end != std::string::npos);
  const std::string formatter = html.substr(fmt, fmt_end - fmt);
  CHECK(formatter.find("const colon = ':';") != std::string::npos);
}

TEST_CASE("web mirror: countdown colon flashes each second when pregame blink enabled") {
  std::string html = read_file(repo_root() + "/external_components/web_editor/web_ui.html");
  REQUIRE(!html.empty());
  html = strip_cr(html);

  const size_t wag = html.find("const colons = mc.querySelectorAll('.cd-colon')");
  REQUIRE(wag != std::string::npos);
  const size_t wag_end = html.find("\n          }", wag);
  REQUIRE(wag_end != std::string::npos);
  const std::string block = html.substr(wag, wag_end - wag);

  CHECK(block.find("for (const colon of colons)") != std::string::npos);
  CHECK(block.find("colon.style.opacity = toggle ? '0' : '1'") != std::string::npos);
  CHECK(block.find("Math.floor(performance.now() / 1000) % 2 === 0") != std::string::npos);
  // Old wig-wag code removed
  CHECK(block.find("wag-top") == std::string::npos);
  CHECK(block.find("wag-bottom") == std::string::npos);
  CHECK(block.find("alignSelf") == std::string::npos);
}

TEST_CASE("generated web UI header is release-stamped and browser-safe") {
  const std::string root = repo_root();
  std::string version = strip_cr(read_file(root + "/external_components/web_editor/VERSION"));
  version.erase(std::remove(version.begin(), version.end(), '\n'), version.end());
  REQUIRE(!version.empty());

  const std::string stamped = "v" + version;
  const std::string yaml = strip_cr(read_file(root + "/esphome/curling-timer.yaml"));
  CHECK(yaml.find("ref: " + stamped) != std::string::npos);
  const std::string ref = strip_cr(read_file(root + "/esphome/component_ref.txt"));
  CHECK(ref.find(stamped) != std::string::npos);

  const std::string source = strip_cr(read_file(root + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!source.empty());
  const std::string expected = replace_all(source, "{{version}}", stamped);
  CHECK(expected.find(stamped) != std::string::npos);

  const std::string header = strip_cr(read_file(root + "/external_components/web_editor/web_ui.h"));
  REQUIRE(!header.empty());
  const std::string open = "R\"esphome_web_ui(";
  const std::string close = ")esphome_web_ui\";";
  const size_t begin = header.find(open);
  REQUIRE(begin != std::string::npos);
  const size_t end = header.rfind(close);
  REQUIRE(end != std::string::npos);
  REQUIRE(end > begin);
  const std::string embedded = header.substr(begin + open.size(), end - (begin + open.size()));

  // The generated artifact actually served by firmware must match the source
  // and carry the same release stamp, not just the HTML template.
  CHECK(embedded == expected);
  CHECK(embedded.find("function browserCountdown(") != std::string::npos);
  CHECK(embedded.find("mc.textContent = browserCountdown(s.countdown);") != std::string::npos);
  CHECK(embedded.find("\xCB\xB8") == std::string::npos);
}
