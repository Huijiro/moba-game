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

int GameSettings::get_casting_mode() {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) {
    return static_cast<int>(CastingMode::INSTANT);  // Default
  }

  godot::Variant value = settings->get_setting(SETTING_CASTING_MODE);
  return value.operator int();
}

void GameSettings::set_casting_mode(int mode) {
  ProjectSettings* settings = ProjectSettings::get_singleton();
  if (settings == nullptr) {
    godot::print_error("[GameSettings] ProjectSettings unavailable");
    return;
  }

  settings->set_setting(SETTING_CASTING_MODE, mode);

  godot::String mode_name = "Unknown";
  switch (static_cast<CastingMode>(mode)) {
    case CastingMode::INSTANT:
      mode_name = "INSTANT";
      break;
    case CastingMode::CLICK_TO_CAST:
      mode_name = "CLICK_TO_CAST";
      break;
    case CastingMode::INDICATOR:
      mode_name = "INDICATOR";
      break;
  }

  UtilityFunctions::print("[GameSettings] Casting mode set to: " + mode_name);
}

CastingMode GameSettings::get_casting_mode_enum() {
  return static_cast<CastingMode>(get_casting_mode());
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

  // Register casting mode setting with default value (INSTANT)
  if (!settings->has_setting(SETTING_CASTING_MODE)) {
    settings->set_setting(SETTING_CASTING_MODE,
                          static_cast<int>(CastingMode::INSTANT));
    UtilityFunctions::print("[GameSettings] Registered casting_mode = INSTANT");
  }
}
