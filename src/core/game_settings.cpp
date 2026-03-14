#include "game_settings.hpp"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/print_string.hpp>

#include "../debug/debug_macros.hpp"

using godot::ProjectSettings;
using godot::String;

bool GameSettings::get_channel_requires_stop_command_only() {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) return true;
  return settings->get_setting(SETTING_CHANNEL_REQUIRES_STOP).operator bool();
}

void GameSettings::set_channel_requires_stop_command_only(bool value) {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) return;
  settings->set_setting(SETTING_CHANNEL_REQUIRES_STOP, value);
}

String GameSettings::get_casting_mode() {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) return "click_to_cast";
  return settings->get_setting(SETTING_CASTING_MODE).operator String();
}

void GameSettings::set_casting_mode(const String& mode) {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) return;
  settings->set_setting(SETTING_CASTING_MODE, mode);
  DBG_INFO("GameSettings", "Casting mode set to: " + mode);
}

CastingMode GameSettings::get_casting_mode_enum() {
  return casting_mode_from_string(get_casting_mode());
}

void GameSettings::register_settings() {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) return;

  if (!settings->has_setting(SETTING_CHANNEL_REQUIRES_STOP)) {
    settings->set_setting(SETTING_CHANNEL_REQUIRES_STOP, true);
  }

  if (!settings->has_setting(SETTING_CASTING_MODE)) {
    settings->set_setting(SETTING_CASTING_MODE, String("click_to_cast"));
  }

  // Add property info for a dropdown hint in the editor
  settings->set_initial_value(SETTING_CASTING_MODE, String("click_to_cast"));
  godot::Dictionary hint;
  hint["name"] = SETTING_CASTING_MODE;
  hint["type"] = godot::Variant::STRING;
  hint["hint"] = godot::PROPERTY_HINT_ENUM;
  hint["hint_string"] = "instant,click_to_cast,indicator";
  settings->add_property_info(hint);

  settings->set_initial_value(SETTING_CHANNEL_REQUIRES_STOP, true);
}
