// Regression guard for the WebUI live mirror's progress bar.
//
// HDMI draws ONE white pill with four weighted colored bands that grow from
// their left edges. The browser mirror used to render one fully-colored bar
// per end with a dark overlay; it must now mirror the HDMI semantics using
// the same snapshot fields (including progress_skip_multiplier).

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

int count_occurrences(const std::string &haystack, const std::string &needle) {
  int count = 0;
  size_t pos = 0;
  while ((pos = haystack.find(needle, pos)) != std::string::npos) {
    count++;
    pos += needle.size();
  }
  return count;
}

std::vector<int> parse_ints_between(const std::string &source, const std::string &begin,
                                    const std::string &end) {
  std::vector<int> values;
  const size_t start = source.find(begin);
  if (start == std::string::npos)
    return values;
  const size_t stop = source.find(end, start + begin.size());
  if (stop == std::string::npos)
    return values;
  int value = 0;
  bool in_number = false;
  for (size_t i = start + begin.size(); i < stop; i++) {
    const char c = source[i];
    if (c >= '0' && c <= '9') {
      value = value * 10 + (c - '0');
      in_number = true;
    } else if (in_number) {
      values.push_back(value);
      value = 0;
      in_number = false;
    }
  }
  if (in_number)
    values.push_back(value);
  return values;
}

std::string function_body(const std::string &source, const std::string &signature) {
  const size_t start = source.find(signature);
  if (start == std::string::npos)
    return {};
  const size_t end = source.find("\n}\n", start);
  if (end == std::string::npos)
    return {};
  return source.substr(start, end - start);
}

}  // namespace

TEST_CASE("web mirror: one white progress pill replaces the per-end bar stack") {
  const std::string html = strip_cr(read_file(repo_root() + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());

  // Exactly one bar, created once — not endCount bars.
  CHECK(html.find("id: 'mirror-progress-bar'") != std::string::npos);
  CHECK(html.find("for (let e = 0; e < endCount; e++)") == std::string::npos);
  CHECK(count_occurrences(html, "class: 'progress-end-bar'") == 1);

  // No dark overlay semantics remain (the modal backdrop uses the same rgba
  // elsewhere, so only the progress-specific classes/rules are checked here).
  CHECK(html.find("progress-end-fill") == std::string::npos);
  CHECK(html.find("progress-seg") == std::string::npos);

  // HDMI geometry: 85px pill, 8px black frame, 24px radius, white track.
  const size_t css = html.find(".progress-end-bar {");
  REQUIRE(css != std::string::npos);
  const std::string bar_rule = html.substr(css, html.find('\n', css) - css);
  CHECK(bar_rule.find("height: 95px") != std::string::npos);
  CHECK(bar_rule.find("background: #fff") != std::string::npos);
  CHECK(bar_rule.find("border: 8px solid #000") != std::string::npos);
  CHECK(bar_rule.find("border-radius: 24px") != std::string::npos);
  CHECK(bar_rule.find("rgba(") == std::string::npos);
  CHECK(html.find("height:${px(95)}px") != std::string::npos);

  // The centre block reserves the whole HDMI pill (bottom y=-94) plus a gap.
  CHECK(html.find("px(94 + 95 + 8)") != std::string::npos);

  // Browser keeps the requested palette; only the skin changed.
  CHECK(html.find(".progress-band.lead { background: #DC3545; }") != std::string::npos);
  CHECK(html.find(".progress-band.second { background: #FFC107; }") != std::string::npos);
  CHECK(html.find(".progress-band.vice { background: #0DCaf0; }") != std::string::npos);
  CHECK(html.find(".progress-band.skip { background: #198754; }") != std::string::npos);
}

TEST_CASE("web mirror: fill math matches the HDMI progress lambda") {
  const std::string html = strip_cr(read_file(repo_root() + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());

  const std::string tick_fn = function_body(html, "function tick() {");
  REQUIRE(!tick_fn.empty());
  CHECK(tick_fn.find("progress_skip_multiplier") != std::string::npos);
  CHECK(tick_fn.find("Math.floor(dur / endCount)") != std::string::npos);
  CHECK(tick_fn.find("inEnd * skipMult / endDur") != std::string::npos);
  CHECK(tick_fn.find("scaled = progress * totalW") != std::string::npos);
  CHECK(tick_fn.find("weights = [1, 1, 1, skipMult]") != std::string::npos);
  CHECK(tick_fn.find("progress-band[data-band=") != std::string::npos);
  CHECK(tick_fn.find("progress-divider[data-div=") != std::string::npos);
  CHECK(tick_fn.find("Math.round(pct)") == std::string::npos);
  CHECK(tick_fn.find("PROGRESS_BAND_GEOM.map((g) => g[1] / PROGRESS_TRACK_PX * 100)") != std::string::npos);

  const size_t progress = html.find("// One pill, exactly like HDMI:");
  REQUIRE(progress != std::string::npos);
  const std::string build = html.substr(progress, html.find("\n  }\n", progress) - progress);
  CHECK(build.find("const pct = (v) => v / PROGRESS_TRACK_PX * 100;") != std::string::npos);
  CHECK(build.find("left:${pct(PROGRESS_BAND_GEOM[i][0])}%") != std::string::npos);
  CHECK(build.find("left:${pct(PROGRESS_DIV_X[i])}%") != std::string::npos);
  // Geometry must not be derived from the skip multiplier; only fill is.
  CHECK(build.find("weights.map((w) => w / totalW * 100)") == std::string::npos);
}

TEST_CASE("web mirror geometry matches HDMI's fixed pixel slots") {
  const std::string root = repo_root();
  const std::string yaml = strip_cr(read_file(root + "/esphome/curling-timer.yaml"));
  const std::string html = strip_cr(read_file(root + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!yaml.empty());
  REQUIRE(!html.empty());

  const std::vector<int> band_x = parse_ints_between(yaml, "const int band_x[4] = {", "};");
  const std::vector<int> div_x = parse_ints_between(yaml, "const int div_x[3] = {", "};");
  const std::vector<int> full_w = parse_ints_between(yaml, "const int full_w[4] = {", "};");
  REQUIRE(band_x == std::vector<int>({0, 282, 564, 846}));
  REQUIRE(div_x == std::vector<int>({278, 560, 842}));
  REQUIRE(full_w == std::vector<int>({282, 282, 282, 378}));

  // The browser must use exactly those HDMI pixel slots inside 1224px.
  const std::vector<int> web_geom = parse_ints_between(html, "const PROGRESS_BAND_GEOM = [", "];");
  const std::vector<int> web_div = parse_ints_between(html, "const PROGRESS_DIV_X = [", "];");
  CHECK(web_geom == std::vector<int>({0, 282, 282, 282, 564, 282, 846, 378}));
  CHECK(web_div == div_x);
  CHECK(html.find("const PROGRESS_TRACK_PX = 1224;") != std::string::npos);
  CHECK(count_occurrences(html, "const PROGRESS_BAND_GEOM =") == 1);
  CHECK(band_x.back() + full_w.back() == 1224);
}

TEST_CASE("status API exposes the progress skip multiplier to the mirror") {
  const std::string api = strip_cr(read_file(repo_root() + "/external_components/web_editor/api.cpp"));
  REQUIRE(!api.empty());
  CHECK(api.find("doc[\"progress_skip_multiplier\"] = snap.progress_skip_multiplier;") != std::string::npos);
}

TEST_CASE("HDMI bands pre-swap R/B; browser CSS keeps the reference palette") {
  const std::string yaml = strip_cr(read_file(repo_root() + "/esphome/curling-timer.yaml"));
  REQUIRE(!yaml.empty());
  CHECK(yaml.find("bg_color: 0x4535DC") != std::string::npos);  // #DC3545
  CHECK(yaml.find("bg_color: 0x07C1FF") != std::string::npos);  // #FFC107
  CHECK(yaml.find("bg_color: 0xF0CA0D") != std::string::npos);  // #0DCAF0
  CHECK(yaml.find("bg_color: 0x548719") != std::string::npos);  // #198754
  CHECK(yaml.find("bypass render.cpp's hex6()") != std::string::npos);

  const std::string render = strip_cr(read_file(repo_root() + "/external_components/curling_engine/render.cpp"));
  REQUIRE(!render.empty());
  CHECK(render.find("Swap R/B to compensate for LVGL 16-bit BGR vs LT8912B 24-bit RGB") != std::string::npos);

  const std::string html = strip_cr(read_file(repo_root() + "/external_components/web_editor/web_ui.html"));
  REQUIRE(!html.empty());
  CHECK(html.find(".progress-band.lead { background: #DC3545; }") != std::string::npos);
  CHECK(html.find(".progress-band.second { background: #FFC107; }") != std::string::npos);
  CHECK(html.find(".progress-band.vice { background: #0DCaf0; }") != std::string::npos);
  CHECK(html.find(".progress-band.skip { background: #198754; }") != std::string::npos);
}

TEST_CASE("generated web UI header carries the progress-bar fix release stamp") {
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

  CHECK(embedded == expected);
  CHECK(embedded.find("id: 'mirror-progress-bar'") != std::string::npos);
  CHECK(embedded.find("progress-end-fill") == std::string::npos);
  CHECK(embedded.find(stamped) != std::string::npos);
}
