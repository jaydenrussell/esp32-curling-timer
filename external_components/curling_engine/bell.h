#pragma once

// Bell/buzzer actuation core (pitimer display.py: buzz_if_due, ring_buzzer,
// reset_buzzer). Pure logic — host-tested; CurlingEngineComponent::bell_due()
// wraps it with the on-device clock + settings read.
//
// pitimer rings every relay in settings buzzer.relays once when a game's
// buzzer time is reached, still ringing within BUZZER_GRACE_SECONDS if the
// exact zero second was skipped, then turns them off after buzzer.duration.

#include <cstdint>
#include <optional>
#include <string>

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

// pitimer buzz_if_due(): returns the relay bitmask to switch on, or 0 when
// nothing is due. Rings once per (date, start) game; *last_key remembers
// that game across calls. *duration_ms is the buzzer.duration pulse length
// (pitimer: settings.relay_duration) when a ring is due.
int bell_check(const PiTimerSettings &settings, const std::optional<PiTimerGame> &game,
               const std::string &date_iso, std::string *last_key, uint32_t *duration_ms);

}  // namespace curling_engine
}  // namespace esphome
