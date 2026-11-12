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

int relay_duration_for(const PiTimerSettings &settings, const char *event, int relay_index) {
  if (std::string(event) == "start") {
    switch (relay_index) {
      case 1: return settings.relay_start_duration_1;
      case 2: return settings.relay_start_duration_2;
      case 3: return settings.relay_start_duration_3;
      case 4: return settings.relay_start_duration_4;
    }
  } else if (std::string(event) == "countdown") {
    switch (relay_index) {
      case 1: return settings.relay_countdown_duration_1;
      case 2: return settings.relay_countdown_duration_2;
      case 3: return settings.relay_countdown_duration_3;
      case 4: return settings.relay_countdown_duration_4;
    }
  } else {  // "end" / buzzer
    switch (relay_index) {
      case 1: return settings.relay_end_duration_1;
      case 2: return settings.relay_end_duration_2;
      case 3: return settings.relay_end_duration_3;
      case 4: return settings.relay_end_duration_4;
    }
  }
  return 0;
}

}  // namespace

// Duration (seconds) to pulse for an event: per-relay overrides win (longest
// when several relays ring together, since the driver pulses all relays for
// one length), falling back to the event-level duration then the global
// buzzer.duration. RELAY3/RELAY4 have no hardware here and are ignored.
int event_duration(const PiTimerSettings &settings, const char *event,
                   const std::vector<std::string> &relays) {
  int best = 0;
  for (const std::string &r : relays) {
    int i = 0;
    if (r == "RELAY1") i = 1;
    else if (r == "RELAY2") i = 2;
    else continue;
    int d = relay_duration_for(settings, event, i);
    if (d > best) best = d;
  }
  if (best > 0) return best;
  if (std::string(event) == "start") return settings.relay_start_duration;
  if (std::string(event) == "countdown") return settings.relay_countdown_duration;
  return settings.relay_end_duration;
}

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
  if ((last_key != nullptr && key == *last_key) || until < -BUZZER_GRACE_SECONDS || until > 0)
    return 0;
  if (last_key != nullptr)
    *last_key = key;
  int mask = 0;
  if (settings.relays.has_value())
    mask = relays_mask(*settings.relays);
  if (mask != 0 && duration_ms != nullptr) {
    int dur = event_duration(settings, "end", *settings.relays);
    if (dur <= 0) dur = settings.relay_duration;
    *duration_ms = static_cast<uint32_t>(dur) * 1000U;
  }
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
  // Per-game start_relays/start_duration override wins over settings.
  if (since_start >= 0 && since_start <= BUZZER_GRACE_SECONDS && fires("start")) {
    const std::vector<std::string> *start_relays = nullptr;
    if (game->start_relays_override.has_value() && !game->start_relays_override->empty())
      start_relays = &(*game->start_relays_override);
    else if (settings.ring_at_start.has_value())
      start_relays = &(*settings.ring_at_start);
    if (start_relays != nullptr && !start_relays->empty()) {
      mask |= relays_mask(*start_relays);
      if (game->start_duration_override.has_value() && *game->start_duration_override > 0) {
        if (static_cast<uint32_t>(*game->start_duration_override) * 1000U > out->duration_ms)
          out->duration_ms = static_cast<uint32_t>(*game->start_duration_override) * 1000U;
      } else {
        int dur = event_duration(settings, "start", *start_relays);
        if (dur > 0 && static_cast<uint32_t>(dur) * 1000U > out->duration_ms)
          out->duration_ms = static_cast<uint32_t>(dur) * 1000U;
      }
    }
  }
  // Countdown warning: seconds_until_buzzer descending into [target-grace, target].
  if (settings.countdown_minutes_before > 0 && !settings.countdown_relays.empty()) {
    const int target = settings.countdown_minutes_before * 60;
    if (settings.countdown_minutes_before > 0 && until <= target && until >= target - BUZZER_GRACE_SECONDS && fires("countdown")) {
      mask |= relays_mask(settings.countdown_relays);
      int dur = event_duration(settings, "countdown", settings.countdown_relays);
      if (dur > 0 && static_cast<uint32_t>(dur) * 1000U > out->duration_ms) out->duration_ms = static_cast<uint32_t>(dur) * 1000U;
    }
  }
  // Buzzer (pitimer buzz_if_due). Per-game end_relays/end_duration override wins.
  if (until >= -1 && until <= 0 && fires("buzzer")) {
    const std::vector<std::string> *end_relays = nullptr;
    if (game->end_relays_override.has_value() && !game->end_relays_override->empty())
      end_relays = &(*game->end_relays_override);
    else if (settings.relays.has_value())
      end_relays = &(*settings.relays);
    if (end_relays != nullptr && !end_relays->empty()) {
      mask |= relays_mask(*end_relays);
      if (game->end_duration_override.has_value() && *game->end_duration_override > 0) {
        if (static_cast<uint32_t>(*game->end_duration_override) * 1000U > out->duration_ms)
          out->duration_ms = static_cast<uint32_t>(*game->end_duration_override) * 1000U;
      } else {
        int dur = event_duration(settings, "end", *end_relays);
        if (dur > 0 && static_cast<uint32_t>(dur) * 1000U > out->duration_ms)
          out->duration_ms = static_cast<uint32_t>(dur) * 1000U;
      }
    }
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
