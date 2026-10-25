#include "bell.h"

namespace esphome {
namespace curling_engine {

namespace {

int relays_mask(const std::vector<std::string> &relays) {
  int mask = 0;
  for (const std::string &relay : relays) {
    if (relay == "RELAY1")
      mask |= BELL_RELAY_1;
    else if (relay == "RELAY2")
      mask |= BELL_RELAY_2;
    // RELAY3/RELAY4: pitimer-only hardware, ignored here.
  }
  return mask;
}

// The fired-event log grows with every game, so keep only the recent tail.
// Keys are "date/start/event" with ISO dates, so the oldest entries sort
// first and the running game (today) is never dropped.
constexpr size_t MAX_FIRED_KEYS = 64;

void prune_fired(std::set<std::string> *fired) {
  while (fired->size() > MAX_FIRED_KEYS)
    fired->erase(fired->begin());
}

}  // namespace

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
  if (settings.relays.has_value())
    mask = relays_mask(*settings.relays);
  if (mask != 0 && duration_ms != nullptr)
    *duration_ms = settings.relay_end_duration > 0 ? settings.relay_end_duration * 1000 : (settings.relay_duration > 0 ? settings.relay_duration * 1000 : 0);
  return mask;
}

bool bell_tick(const PiTimerSettings &settings, const std::optional<PiTimerGame> &game,
               const std::string &date_iso, std::set<std::string> *fired, BellRing *out) {
  *out = BellRing{};
  if (!settings.enabled || !game.has_value() || !game->seconds_until_buzzer.has_value())
    return false;
  const int until = *game->seconds_until_buzzer;
  const int since_start = game->seconds_since_start.value_or(0);
  const std::string base = date_iso + "/" + std::to_string(game->start);
  auto fires = [&](const char *event) {
    const bool first_time = fired->insert(base + "/" + event).second;
    prune_fired(fired);
    return first_time;  // true = first time for this game
  };
  int mask = 0;
  // Game start: seconds_since_start in [0, grace] (skip-tolerant like buzzer).
  if (settings.ring_at_start.has_value() && !settings.ring_at_start->empty() && settings.relay_start_duration > 0 && since_start >= 0 &&
      since_start <= BUZZER_GRACE_SECONDS && fires("start")) {
    mask |= relays_mask(*settings.ring_at_start);
    if (settings.relay_start_duration * 1000 > out->duration_ms) out->duration_ms = settings.relay_start_duration * 1000;
  }
  // Countdown warning: seconds_until_buzzer descending into [target-grace, target].
  if (settings.countdown_minutes_before > 0 && !settings.countdown_relays.empty()) {
    const int target = settings.countdown_minutes_before * 60;
    if (settings.relay_countdown_duration > 0 && until <= target && until >= target - BUZZER_GRACE_SECONDS && fires("countdown")) {
      mask |= relays_mask(settings.countdown_relays);
      if (settings.relay_countdown_duration * 1000 > out->duration_ms) out->duration_ms = settings.relay_countdown_duration * 1000;
    }
  }
  // Buzzer (pitimer buzz_if_due).
  if (until >= -1 && until <= 0 && settings.relay_end_duration > 0 && fires("buzzer") && settings.relays.has_value()) {
    mask |= relays_mask(*settings.relays);
    if (settings.relay_end_duration * 1000 > out->duration_ms) out->duration_ms = settings.relay_end_duration * 1000;
  }
  if (mask == 0)
    return false;
  out->mask = mask;
  if (out->duration_ms == 0)
    return false;
  return true;
}

}  // namespace curling_engine
}  // namespace esphome
