// Regression guards for the browser mirror's editable Quick Picks workflow.

#include "doctest.h"

#include <algorithm>
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

std::string quick_presets_block(const std::string &html) {
  const size_t begin = html.find("const QUICK_PRESETS = [");
  if (begin == std::string::npos)
    return {};
  const size_t end = html.find("\n];", begin);
  if (end == std::string::npos)
    return {};
  return html.substr(begin, end - begin);
}

}  // namespace

TEST_CASE("web quick picks: countdown label uses pre-game wording") {
  const std::string html = strip_cr(read_file(repo_root() + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());

  CHECK(html.find("Wig Wag Countdown") == std::string::npos);
  CHECK(html.find("Pre-Game Countdown") != std::string::npos);
}

TEST_CASE("web quick picks: buttons populate the form instead of launching") {
  const std::string html = strip_cr(read_file(repo_root() + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());

  CHECK(html.find("launchQuick({ seconds: preset.seconds") == std::string::npos);
  CHECK(html.find("applyQuickPreset") != std::string::npos);
  CHECK(html.find("const applyQuickPreset = (preset) =>") != std::string::npos);
  CHECK(html.find("onclick: () => applyQuickPreset(preset)") != std::string::npos);
}

TEST_CASE("web quick picks: built-in presets carry the complete preset shape") {
  const std::string html = strip_cr(read_file(repo_root() + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());
  const std::string block = quick_presets_block(html);
  REQUIRE(!block.empty());

  const std::vector<std::string> fields = {
    "id:", "label:", "name:", "game_completed:", "minutes:", "pregame_minutes:",
    "header_hold_minutes:", "stones_per_end:", "end_count:", "untimed_ends:",
    "options:", "pregame:", "pace_of_play:", "show_progress_through_end:",
    "total_time_includes_untimed_ends:", "replace_conflicts:",
    "start_relays:", "start_duration:", "end_relays:", "end_duration:",
  };
  for (const std::string &field : fields)
    CHECK(block.find(field) != std::string::npos);

  const std::vector<std::string> ids = {"practice", "game-6", "game-8", "doubles-8"};
  for (size_t i = 0; i < ids.size(); i++) {
    const size_t begin = block.find("id: '" + ids[i] + "'");
    REQUIRE(begin != std::string::npos);
    const size_t end = i + 1 < ids.size() ? block.find("id: '" + ids[i + 1] + "'", begin) : block.size();
    REQUIRE(end != std::string::npos);
    const std::string preset = block.substr(begin, end - begin);
    for (const std::string &field : fields)
      CHECK(preset.find(field) != std::string::npos);
  }
}

TEST_CASE("web quick picks: persistence and completed-message payload are wired") {
  const std::string html = strip_cr(read_file(repo_root() + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());

  CHECK(html.find("curling_timer.quick_presets.v1") != std::string::npos);
  CHECK(html.find("localStorage.getItem(QUICK_PRESET_STORAGE_KEY)") != std::string::npos);
  CHECK(html.find("localStorage.setItem(QUICK_PRESET_STORAGE_KEY") != std::string::npos);
  CHECK(html.find("const renderQuickPicks = () =>") != std::string::npos);
  CHECK(html.find("Save as quick pick") != std::string::npos);
  CHECK(html.find("Reset quick picks") != std::string::npos);
  CHECK(html.find("'aria-label': 'Completed message'") != std::string::npos);
  CHECK(html.find("...(completedMessage ? { game_completed: completedMessage } : {}),") != std::string::npos);
}

TEST_CASE("web quick picks: generated header matches the edited source") {
  const std::string root = repo_root();
  std::string version = strip_cr(read_file(root + "/external_components/web_editor/VERSION"));
  version.erase(std::remove(version.begin(), version.end(), '\n'), version.end());
  REQUIRE(!version.empty());

  const std::string source = strip_cr(read_file(root + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!source.empty());
  const std::string expected = replace_all(source, "{{version}}", "v" + version);

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

  CHECK(embedded == expected);
  CHECK(embedded.find("applyQuickPreset") != std::string::npos);
}
