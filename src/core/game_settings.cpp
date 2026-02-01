#include "game_settings.hpp"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::ProjectSettings;
using godot::UtilityFunctions;

bool GameSettings::get_channel_requires_stop_command_only() {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) {
    return true;  // Default to true if ProjectSettings unavailable
  }

  godot::Variant value = settings->get_setting(SETTING_CHANNEL_REQUIRES_STOP);
  return value.operator bool();
}

void GameSettings::set_channel_requires_stop_command_only(bool value) {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) {
    godot::print_error("[GameSettings] ProjectSettings unavailable");
    return;
  }

  settings->set_setting(SETTING_CHANNEL_REQUIRES_STOP, value);
  UtilityFunctions::print(
      "[GameSettings] Channel requires stop command only: " +
      godot::String(value ? "true" : "false"));
}

void GameSettings::register_settings() {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) {
    godot::print_error(
        "[GameSettings] Cannot register settings - ProjectSettings "
        "unavailable");
    return;
  }

  // Register channel setting with default value
  if (!settings->has_setting(SETTING_CHANNEL_REQUIRES_STOP)) {
    settings->set_setting(SETTING_CHANNEL_REQUIRES_STOP, true);
    UtilityFunctions::print(
        "[GameSettings] Registered channel_requires_stop_command_only = true");
  }
}
