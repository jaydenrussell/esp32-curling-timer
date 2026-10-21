#pragma once

// Bell/buzzer actuation core (pitimer display.py: buzz_if_due, ring_buzzer,
// reset_buzzer) extended with the game-start and countdown-warning ring
// events. Pure logic — host-tested; CurlingEngineComponent::bell_tick()
// wraps it with the on-device clock + settings read.
//
// Events (config.buzzer in the settings file, each with its own relay list):
//   buzzer    - pitimer buzz_if_due: rings once at the game's buzzer time,
//               still within BUZZER_GRACE_SECONDS if the zero second was
//               skipped. relays: [...]              (pitimer field)
//   start     - optional: rings when a game starts. ring_at_start: [...]
//   countdown - optional: rings minutes_before the buzzer.
//               countdown: {minutes_before: N, relays: [...]}
// Each event rings once per (date, start) game. All use buzzer.duration.

#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "settings.h"

namespace esphome {
namespace curling_engine {

// pitimer display.py BUZZER_GRACE_SECONDS
static constexpr int BUZZER_GRACE_SECONDS = 5;

// Physical relay bits on the M5Stack Unit 2Relay (Grove Port B). These
// correspond to pitimer's RELAY1/RELAY2 PiRelayV2 names; RELAY3/RELAY4
// have no hardware here and are ignored.
static constexpr int BELL_RELAY_1 = 1;
static constexpr int BELL_RELAY_2 = 2;

// One merged ring decision for the current tick.
struct BellRing {
  int mask{0};          // BELL_RELAY_* bits to switch on
  uint32_t duration_ms{0};  // pulse length (settings buzzer.duration)
};

// pitimer buzz_if_due(): returns the relay bitmask to switch on, or 0 when
// nothing is due. Rings once per (date, start) game; *last_key remembers
// that game across calls. *duration_ms is the buzzer.duration pulse length
// (pitimer: settings.relay_duration) when a ring is due.
int bell_check(const PiTimerSettings &settings, const std::optional<PiTimerGame> &game,
               const std::string &date_iso, std::string *last_key, uint32_t *duration_ms);

// Extended tick: evaluates the buzzer event plus the optional game-start and
// countdown-warning events. Returns true when at least one event fired this
// tick (*out merged: OR of relay masks, buzzer.duration). *fired accumulates
// "date/start/event" keys so each event rings once per game.
bool bell_tick(const PiTimerSettings &settings, const std::optional<PiTimerGame> &game,
               const std::string &date_iso, std::set<std::string> *fired, BellRing *out);

}  // namespace curling_engine
}  // namespace esphome
