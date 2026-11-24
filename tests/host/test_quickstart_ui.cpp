// Regression guard for the manual Quick Game form silently dropping fields.
//
// The form computes pregame_countdown (and the per-game relay/end overrides),
// but launchQuick() used to POST only a fixed set of fields plus the boolean
// options, so the server fell back to the global pre-game setting and ignored
// the value the user entered. Presets still worked because they intentionally
// use that global fallback, which made the bug easy to miss.

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

TEST_CASE("quick game: the manual form's payload reaches the quickstart API") {
  std::string html = read_file(repo_root() + "/external_components/web_editor/web_ui.html");
  REQUIRE(!html.empty());
  html.erase(std::remove(html.begin(), html.end(), '\r'), html.end());

  const size_t fn = html.find("async function launchQuick(payload");
  REQUIRE(fn != std::string::npos);
  const size_t fn_end = html.find("\n}\n", fn);
  REQUIRE(fn_end != std::string::npos);
  const std::string body = html.substr(fn, fn_end - fn);

  // Forward the whole payload, not a hand-picked subset: every field the form
  // computes (including pregame_countdown in seconds) must reach the server.
  CHECK(body.find("{ ...payload") != std::string::npos);
  CHECK(body.find("api('/api/quickstart', request)") != std::string::npos);

  // The manual submit still supplies the per-game pre-game countdown.
  const size_t pregame = html.find("pregame_countdown:");
  REQUIRE(pregame != std::string::npos);
  const size_t submit = html.rfind("launchQuick({", pregame);
  REQUIRE(submit != std::string::npos);
  CHECK(pregame - submit < 600);
}
