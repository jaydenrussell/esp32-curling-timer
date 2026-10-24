#include "render.h"

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace esphome {
namespace curling_engine {

namespace {

struct ColorName {
  const char *name;
  uint32_t rgb;
};

// pygame.Color accepts CSS3/SVG colour names (case-insensitive).
constexpr ColorName kColorNames[] = {
    {"aliceblue", 0xf0f8ff},      {"antiquewhite", 0xfaebd7}, {"aqua", 0x00ffff},
    {"aquamarine", 0x7fffd4},     {"azure", 0xf0ffff},        {"beige", 0xf5f5dc},
    {"bisque", 0xffe4c4},         {"black", 0x000000},        {"blanchedalmond", 0xffebcd},
    {"blue", 0x0000ff},           {"blueviolet", 0x8a2be2},   {"brown", 0xa52a2a},
    {"burlywood", 0xdeb887},      {"cadetblue", 0x5f9ea0},    {"chartreuse", 0x7fff00},
    {"chocolate", 0xd2691e},      {"coral", 0xff7f50},        {"cornflowerblue", 0x6495ed},
    {"cornsilk", 0xfff8dc},       {"crimson", 0xdc143c},      {"cyan", 0x00ffff},
    {"darkblue", 0x00008b},       {"darkcyan", 0x008b8b},     {"darkgoldenrod", 0xb8860b},
    {"darkgray", 0xa9a9a9},       {"darkgrey", 0xa9a9a9},     {"darkgreen", 0x006400},
    {"darkkhaki", 0xbdb76b},      {"darkmagenta", 0x8b008b},  {"darkolivegreen", 0x556b2f},
    {"darkorange", 0xff8c00},     {"darkorchid", 0x9932cc},   {"darkred", 0x8b0000},
    {"darksalmon", 0xe9967a},     {"darkseagreen", 0x8fbc8f}, {"darkslateblue", 0x483d8b},
    {"darkslategray", 0x2f4f4f},  {"darkslategrey", 0x2f4f4f}, {"darkturquoise", 0x00ced1},
    {"darkviolet", 0x9400d3},     {"deeppink", 0xff1493},     {"deepskyblue", 0x00bfff},
    {"dimgray", 0x696969},        {"dimgrey", 0x696969},      {"dodgerblue", 0x1e90ff},
    {"firebrick", 0xb22222},      {"floralwhite", 0xfffaf0},  {"forestgreen", 0x228b22},
    {"fuchsia", 0xff00ff},        {"gainsboro", 0xdcdcdc},    {"ghostwhite", 0xf8f8ff},
    {"gold", 0xffd700},           {"goldenrod", 0xdaa520},    {"gray", 0x808080},
    {"grey", 0x808080},           {"green", 0x008000},        {"greenyellow", 0xadff2f},
    {"honeydew", 0xf0fff0},       {"hotpink", 0xff69b4},      {"indianred", 0xcd5c5c},
    {"indigo", 0x4b0082},         {"ivory", 0xfffff0},        {"khaki", 0xf0e68c},
    {"lavender", 0xe6e6fa},       {"lavenderblush", 0xfff0f5}, {"lawngreen", 0x7cfc00},
    {"lemonchiffon", 0xfffacd},   {"lightblue", 0xadd8e6},    {"lightcoral", 0xf08080},
    {"lightcyan", 0xe0ffff},      {"lightgoldenrodyellow", 0xfafad2}, {"lightgray", 0xd3d3d3},
    {"lightgrey", 0xd3d3d3},      {"lightgreen", 0x90ee90},   {"lightpink", 0xffb6c1},
    {"lightsalmon", 0xffa07a},    {"lightseagreen", 0x20b2aa}, {"lightskyblue", 0x87cefa},
    {"lightslategray", 0x778899}, {"lightslategrey", 0x778899}, {"lightsteelblue", 0xb0c4de},
    {"lightyellow", 0xffffe0},    {"lime", 0x00ff00},         {"limegreen", 0x32cd32},
    {"linen", 0xfaf0e6},          {"magenta", 0xff00ff},      {"maroon", 0x800000},
    {"mediumaquamarine", 0x66cdaa}, {"mediumblue", 0x0000cd}, {"mediumorchid", 0xba55d3},
    {"mediumpurple", 0x9370db},   {"mediumseagreen", 0x3cb371}, {"mediumslateblue", 0x7b68ee},
    {"mediumspringgreen", 0x00fa9a}, {"mediumturquoise", 0x48d1cc}, {"mediumvioletred", 0xc71585},
    {"midnightblue", 0x191970},   {"mintcream", 0xf5fffa},    {"mistyrose", 0xffe4e1},
    {"moccasin", 0xffe4b5},       {"navajowhite", 0xffdead},  {"navy", 0x000080},
    {"oldlace", 0xfdf5e6},        {"olive", 0x808000},        {"olivedrab", 0x6b8e23},
    {"orange", 0xffa500},         {"orangered", 0xff4500},    {"orchid", 0xda70d6},
    {"palegoldenrod", 0xeee8aa},  {"palegreen", 0x98fb98},    {"paleturquoise", 0xafeeee},
    {"palevioletred", 0xdb7093},  {"papayawhip", 0xffefd5},   {"peachpuff", 0xffdab9},
    {"peru", 0xcd853f},           {"pink", 0xffc0cb},         {"plum", 0xdda0dd},
    {"powderblue", 0xb0e0e6},     {"purple", 0x800080},       {"rebeccapurple", 0x663399},
    {"red", 0xff0000},            {"rosybrown", 0xbc8f8f},    {"royalblue", 0x4169e1},
    {"saddlebrown", 0x8b4513},    {"salmon", 0xfa8072},       {"sandybrown", 0xf4a460},
    {"seagreen", 0x2e8b57},       {"seashell", 0xfff5ee},     {"sienna", 0xa0522d},
    {"silver", 0xc0c0c0},         {"skyblue", 0x87ceeb},      {"slateblue", 0x6a5acd},
    {"slategray", 0x708090},      {"slategrey", 0x708090},    {"snow", 0xfffafa},
    {"springgreen", 0x00ff7f},    {"steelblue", 0x4682b4},    {"tan", 0xd2b48c},
    {"teal", 0x008080},           {"thistle", 0xd8bfd8},      {"tomato", 0xff6347},
    {"turquoise", 0x40e0d0},      {"violet", 0xee82ee},       {"wheat", 0xf5deb3},
    {"white", 0xffffff},          {"whitesmoke", 0xf5f5f5},   {"yellow", 0xffff00},
    {"yellowgreen", 0x9acd32},
};

std::string hex6(uint32_t rgb) {
  // Swap R/B to compensate for LVGL 16-bit BGR vs LT8912B 24-bit RGB
  uint32_t swapped = ((rgb & 0xFF) << 16) | (rgb & 0x00FF00) | ((rgb >> 16) & 0xFF);
  char buf[8];
  snprintf(buf, sizeof(buf), "#%06X", swapped);
  return std::string(buf);
}

std::string lower_copy(const std::string &in) {
  std::string out = in;
  for (char &c : out)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return out;
}

int hex_digit(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

std::string headers_get(const PiTimerSettings &settings, const char *group,
                        const std::string &event, const char *fallback) {
  auto g = settings.headers.find(group);
  if (g == settings.headers.end())
    return fallback;
  auto v = g->second.find(event);
  if (v == g->second.end())
    return fallback;
  return v->second;
}

const char *kMonthAbbr[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                              "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

}  // namespace

std::string color_to_hex(const std::string &name, const std::string &fallback) {
  std::string s;
  s.reserve(name.size());
  for (char c : name) {
    if (!std::isspace(static_cast<unsigned char>(c)))
      s.push_back(c);
  }
  if (!s.empty() && s[0] == '#') {
    if (s.size() == 4) {  // #rgb
      int r = hex_digit(s[1]), g = hex_digit(s[2]), b = hex_digit(s[3]);
      if (r >= 0 && g >= 0 && b >= 0)
        return hex6((r << 4 | r) << 16 | (g << 4 | g) << 8 | (b << 4 | b));
    } else if (s.size() == 7) {  // #rrggbb
      bool ok = true;
      uint32_t rgb = 0;
      for (int i = 1; i < 7; i++) {
        int d = hex_digit(s[i]);
        if (d < 0) {
          ok = false;
          break;
        }
        rgb = rgb << 4 | d;
      }
      if (ok)
        return hex6(rgb);
    }
    return fallback;
  }
  std::string lname = lower_copy(s);
  for (const ColorName &cn : kColorNames) {
    if (lname == cn.name)
      return hex6(cn.rgb);
  }
  return fallback;
}

std::string format_countdown(int seconds_until_buzzer) {
  if (seconds_until_buzzer <= 0)
    return std::string("00") + RAISED_COLON + "00";
  char buf[48];
  int hours = seconds_until_buzzer / 3600;
  int minutes = (seconds_until_buzzer % 3600) / 60;
  int seconds = seconds_until_buzzer % 60;
  if (hours > 0)
    snprintf(buf, sizeof(buf), "%d%s%02d%s%02d", hours, RAISED_COLON, minutes, RAISED_COLON,
             seconds);
  else
    snprintf(buf, sizeof(buf), "%02d%s%02d", minutes, RAISED_COLON, seconds);
  return buf;
}

std::string pace_position(int seconds_since_start, int unit_duration, int *unit_number) {
  if (unit_duration <= 0) {
    if (unit_number != nullptr)
      *unit_number = 1;
    return "playing";
  }
  int unit = seconds_since_start / unit_duration + 1;
  int into_unit = seconds_since_start % unit_duration;
  if (unit_number != nullptr)
    *unit_number = unit;
  if (into_unit < PACE_EDGE_SECONDS)
    return "starting";
  if (into_unit > unit_duration - PACE_EDGE_SECONDS)
    return "finishing";
  return "playing";
}

std::string header_event(const PiTimerSettings &settings,
                         const std::optional<PiTimerGame> &game) {
  if (!settings.enabled)
    return "when_disabled";
  if (!game.has_value())
    return "when_idle";
  if (game->seconds_until_buzzer.has_value() && *game->seconds_until_buzzer > 0)
    return "counting_down";
  return "during_buzzer";
}

std::string recolor(const std::string &hex, const std::string &text) {
  if (text.empty() || hex.empty())
    return text;
  return hex + " " + text + "#";
}

DisplaySnapshot render_snapshot(const PiTimerSettings &settings, bool clock_known,
                                int weekday_mon0, int seconds_of_day,
                                const std::string &date_iso) {
  DisplaySnapshot out;
  out.valid = settings.valid;
  out.clock_known = clock_known;
  if (!settings.valid)
    return out;

  std::optional<PiTimerGame> game;
  if (clock_known)
    game = settings.find_current_game(weekday_mon0, seconds_of_day, date_iso);

  // draw_header(): event text/colour, with the game's during_buzzer override.
  const std::string event = header_event(settings, game);
  out.header_text = headers_get(settings, "text", event, "");
  if (event == "during_buzzer" && game.has_value() && game->during_buzzer_override.has_value())
    out.header_text = *game->during_buzzer_override;
  out.header_color = color_to_hex(headers_get(settings, "color", event, "white"));
  out.footer_color = color_to_hex(settings.footer_color, FOOTER_COLOR_HEX);

  if (!clock_known) {
    // No sane local time yet: pitimer always had a clock; ask instead of
    // showing a wrong countdown/date.
    out.show_status = true;
    out.status_text = std::string("WAITING FOR TIME") + RAISED_COLON;
    return out;
  }

  // draw_screen(): countdown + optional pace + footer game name need
  // enabled && a current game.
  if (settings.enabled && game.has_value()) {
    const PiTimerGame &g = *game;
    const int since_start = g.seconds_since_start.value_or(0);
    const bool pregame = since_start < 0 && settings.pregame_countdown > 0;
    const int seconds_left = pregame ? (-since_start) : g.seconds_until_buzzer.value_or(0);
    out.show_countdown = true;
    out.during_buzzer = !pregame && seconds_left <= 0;
    out.seconds_until_buzzer = pregame ? (g.seconds_until_buzzer.value_or(0) + settings.pregame_countdown) : seconds_left;
    out.countdown = pregame ? std::to_string(seconds_left) : format_countdown(seconds_left);
    out.countdown_color = color_to_hex(pregame ? settings.pregame_color : [&settings](const char *key) {
          auto it = settings.countdown_colors.find(key);
          return it != settings.countdown_colors.end() ? it->second : std::string("white");
        }(out.during_buzzer ? "during_buzzer" : "counting_down"));
    if (pregame && !settings.pregame_header_text.empty())
      out.header_text = settings.pregame_header_text;

    bool show_pace = settings.pace_of_play;
    if (g.pace_of_play_override.has_value())
      show_pace = *g.pace_of_play_override;
    int pace_unit_duration = settings.pace_unit_duration;
    if (g.pace_unit_override.has_value())
      pace_unit_duration = *g.pace_unit_override;
    if (show_pace && pace_unit_duration > 0 && g.seconds_since_start.has_value()) {
      int unit_number = 1;
      std::string position = pace_position(*g.seconds_since_start, pace_unit_duration, &unit_number);
      out.show_pace = true;
      out.pace_text = "Pace is " + position + " " + settings.pace_unit_name + " " +
                      std::to_string(unit_number);
      out.pace_color = color_to_hex(settings.pace_color);
    }

    if (g.message.has_value() && settings.game_name) {
      out.show_game_name = true;
      out.game_name = *g.message;
    }
  }

  // draw_date_time(): pitimer footer date/time, yellow.
  if (settings.date_and_time && date_iso.size() >= 10) {
    int year = 0, month = 0, day = 0;
    if (sscanf(date_iso.c_str(), "%4d-%2d-%2d", &year, &month, &day) == 3 && month >= 1 &&
        month <= 12) {
      char date_buf[32];
      snprintf(date_buf, sizeof(date_buf), "%04d-%s-%02d", year, kMonthAbbr[month - 1], day);
      out.date_text = date_buf;
      char time_buf[32];
      snprintf(time_buf, sizeof(time_buf), "%02d%s%02d%s%02d", seconds_of_day / 3600, RAISED_COLON,
               (seconds_of_day % 3600) / 60, RAISED_COLON, seconds_of_day % 60);
      out.time_text = time_buf;
      out.show_date_time = true;
    }
  }

  return out;
}

}  // namespace curling_engine
}  // namespace esphome
