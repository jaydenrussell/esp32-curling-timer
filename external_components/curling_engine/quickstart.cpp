#include "quickstart.h"

#include <algorithm>

#include "settings.h"

namespace esphome {
namespace curling_engine {

namespace {

// editor.py _text_value
bool text_value(const std::optional<std::string> &value, const char *where,
                std::optional<std::string> *out, std::string *err) {
  if (!value.has_value() || value->empty()) {
    out->reset();
    return true;
  }
  if (value->find_first_of("\r\n\t") != std::string::npos) {
    *err = std::string(where) + " must be a single line of text";
    return false;
  }
  *out = *value;
  return true;
}

// editor.py _conflict_name — e.g. "'DAY LEAGUE' (13:00:00-14:45:00, 30s hold)"
std::string conflict_name(const Entry &entry) {
  std::string own;
  if (entry.header_hold.has_value())
    own = ", " + std::to_string(*entry.header_hold) + "s hold";
  const std::string name =
      (entry.name.has_value() && !entry.name->empty()) ? *entry.name : std::string("unnamed");
  return "'" + name + "' (" + format_time(entry.start) + "-" + format_time(entry.buzzer) + own +
         ")";
}

// editor.py _conflict_message
std::string conflict_message(const Entry &new_entry, const std::vector<Entry> &conflicts,
                             const std::vector<Entry> &entries, int hold) {
  std::string names;
  for (const Entry &c : conflicts) {
    if (!names.empty())
      names += ", ";
    names += conflict_name(c);
  }
  std::string message = "overlaps " + names + " including the header hold";
  for (const Entry &e : entries) {
    if (e.start <= new_entry.start && new_entry.start <= e.buzzer + entry_hold(e, hold))
      return message +
             "; a game is in progress, tick \"replace conflicting games\" to override it";
  }
  int next_start = conflicts.front().start;
  for (const Entry &c : conflicts)
    next_start = std::min(next_start, c.start);
  // the new timer's own hold has to end before the next game starts
  int longest = next_start - entry_hold(new_entry, hold) - 1 - new_entry.start;
  if (longest >= 1)
    return message + "; the longest timer that fits now is " + format_duration(longest);
  return message + "; there is no room before the next game";
}

// editor.py _quick_entry
bool quick_entry(const QuickStartOptions &opts, Entry *out, std::string *err) {
  if (opts.duration <= 0) {
    *err = "duration must be greater than 0";
    return false;
  }
  if (opts.duration >= MAX_GAME_SECONDS) {
    *err = "duration must be less than " + format_duration(MAX_GAME_SECONDS);
    return false;
  }
  if (opts.header_hold.has_value() && *opts.header_hold < 0) {
    *err = "header_hold must be a whole number of seconds (0 or more)";
    return false;
  }
  int start = opts.now_sec + opts.pregame_countdown;
  if (!opts.allow_midnight_wrap && start + opts.duration >= SECONDS_PER_DAY) {
    *err = "a " + format_duration(opts.duration) + " timer would end after midnight (at most " +
           format_duration(SECONDS_PER_DAY - 1 - start) + " left today)";
    return false;
  }
  Entry entry;
  entry.start = start;
  entry.buzzer = start + opts.duration;
  std::optional<std::string> name;
  if (!text_value(opts.name, "name", &name, err))
    return false;
  entry.name = name.has_value() ? name : std::optional<std::string>(QUICK_DEFAULT_NAME);
  if (!text_value(opts.during_buzzer, "during_buzzer", &entry.during_buzzer, err))
    return false;
  entry.header_hold = opts.header_hold;
  entry.pace_of_play = opts.pace_of_play;
  entry.quick = true;
  *out = std::move(entry);
  return true;
}

}  // namespace

std::string format_duration(int seconds) {
  int minutes = seconds / 60;
  int secs = seconds % 60;
  std::string out;
  if (minutes != 0)
    out = std::to_string(minutes) + " min";
  if (secs != 0 || minutes == 0) {
    if (!out.empty())
      out += " ";
    out += std::to_string(secs) + " s";
  }
  return out;
}

bool quick_start(ScheduleModel *model, const QuickStartOptions &opts, Entry *out, std::string *err) {
  Entry new_entry;
  if (!quick_entry(opts, &new_entry, err))
    return false;
  ExceptionDay *day = model->exception_for(opts.date_iso);
  std::vector<Entry> entries;
  if (day != nullptr) {
    entries = day->entries;
  } else {
    entries = model->entries_for(opts.date_iso);
  }
  std::vector<Entry> conflicts;
  for (const Entry &e : entries) {
    if (entries_overlap(new_entry, e, opts.hold))
      conflicts.push_back(e);
  }
  if (!conflicts.empty() && !opts.replace_conflicts) {
    *err = conflict_message(new_entry, conflicts, entries, opts.hold);
    return false;
  }
  std::vector<Entry> kept;
  for (const Entry &e : entries) {
    bool conflicted = false;
    for (const Entry &c : conflicts)
      if (e == c)
        conflicted = true;
    if (!conflicted)
      kept.push_back(e);
  }
  kept.push_back(new_entry);
  std::stable_sort(kept.begin(), kept.end(),
                   [](const Entry &a, const Entry &b) { return a.start < b.start; });

  if (day == nullptr) {
    ExceptionDay fresh{opts.date_iso, QUICK_NOTE, {}};
    // keep the dates in order when the file already is
    size_t later = model->exceptions.size();
    for (size_t i = 0; i < model->exceptions.size(); i++) {
      if (model->exceptions[i].date > fresh.date) {
        later = i;
        break;
      }
    }
    model->exceptions.insert(model->exceptions.begin() + static_cast<long>(later), fresh);
    day = model->exception_for(opts.date_iso);
  }
  day->entries = std::move(kept);
  *out = std::move(new_entry);
  return true;
}

bool cancel_quick_start(ScheduleModel *model, const std::string &date_iso, int start,
                        std::string *err) {
  ExceptionDay *day = model->exception_for(date_iso);
  std::vector<Entry>::iterator match;
  bool found = false;
  if (day != nullptr) {
    for (auto it = day->entries.begin(); it != day->entries.end(); ++it) {
      if (it->quick && it->start == start) {
        match = it;
        found = true;
        break;
      }
    }
  }
  if (day == nullptr || !found) {
    *err = "no quick start timer found to cancel";
    return false;
  }
  day->entries.erase(match);
  std::vector<Entry> weekday;
  int wd = weekday_mon0_from_iso(date_iso);
  if (wd >= 0) {
    auto it = model->schedule.find(DAY_NAMES[wd]);
    if (it != model->schedule.end())
      weekday = it->second;
  }
  if (day->note == QUICK_NOTE &&
      std::none_of(day->entries.begin(), day->entries.end(),
                   [](const Entry &e) { return e.quick; }) &&
      day->entries == weekday) {
    for (auto it = model->exceptions.begin(); it != model->exceptions.end(); ++it) {
      if (it->date == date_iso) {
        model->exceptions.erase(it);
        break;
      }
    }
  }
  return true;
}

}  // namespace curling_engine
}  // namespace esphome
