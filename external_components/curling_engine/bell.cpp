#include "bell.h"

namespace esphome {
namespace curling_engine {

int bell_check(const PiTimerSettings &settings, const std::optional<PiTimerGame> &game,
               const std::string &date_iso, std::string *last_key, uint32_t *duration_ms) {
  if (duration_ms != nullptr)
    *duration_ms = 0;
  // pitimer buzz_if_due(): enabled + live game required.
  if (!settings.enabled || !game.has_value() || !game->seconds_until_buzzer.has_value())
    return 0;
  const int until = *game->seconds_until_buzzer;
  // pitimer game_key = (date, start): one buzz per game per day.
  const std::string key = date_iso + "/" + std::to_string(game->start);
  if (key == *last_key || until < -BUZZER_GRACE_SECONDS || until > 0)
    return 0;
  *last_key = key;
  int mask = 0;
  if (settings.relays.has_value()) {
    for (const std::string &relay : *settings.relays) {
      if (relay == "RELAY1")
        mask |= BELL_RELAY_1;
      else if (relay == "RELAY2")
        mask |= BELL_RELAY_2;
      // RELAY3/RELAY4: pitimer-only hardware, ignored here.
    }
  }
  if (mask != 0 && duration_ms != nullptr)
    *duration_ms = static_cast<uint32_t>(settings.relay_duration) * 1000;  // pitimer: seconds
  return mask;
}

}  // namespace curling_engine
}  // namespace esphome
