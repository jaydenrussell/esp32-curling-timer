#pragma once

// JSON forms of the editor model: python web.py/editor.py ScheduleModel and
// Entry to_json/from_json, on ArduinoJson. Device side only (the YAML layer
// is host tested; these messages are asserted against the python source).

#include <string>

#include "esphome/components/curling_engine/model.h"
#include "esphome/components/json/json_util.h"

namespace esphome {
namespace web_editor {

// Entry.__dict__ as JSON (all keys, null for None).
std::string entry_to_json(const curling_engine::Entry &entry);

// Write an Entry into an existing JSON object / array element.
void entry_into_json(JsonObject obj, const curling_engine::Entry &entry);
void add_entry_json(JsonArray array, const curling_engine::Entry &entry);

// ScheduleModel.to_json(): {schedule, exceptions, inner_comments}.
std::string model_to_json(const curling_engine::ScheduleModel &model);

// ScheduleModel.from_json on an already-parsed JSON object; EditorError
// messages ported verbatim.
bool model_from_json(JsonObjectConst data, curling_engine::ScheduleModel *out, std::string *err);

}  // namespace web_editor
}  // namespace esphome
