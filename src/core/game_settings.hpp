#ifndef GDEXTENSION_GAME_SETTINGS_H
#define GDEXTENSION_GAME_SETTINGS_H

#include <godot_cpp/variant/string.hpp>

#include "../common/casting_mode.hpp"

/// Game settings manager using Godot's ProjectSettings
/// Provides centralized access to game configuration
class GameSettings {
 public:
  // Channel ability settings
  static bool get_channel_requires_stop_command_only();
  static void set_channel_requires_stop_command_only(bool value);

  // Casting mode settings (stored as string: "instant", "click_to_cast", "indicator")
  static godot::String get_casting_mode();
  static void set_casting_mode(const godot::String& mode);
  static CastingMode get_casting_mode_enum();

  // Register default settings with ProjectSettings
  static void register_settings();

 private:
  static constexpr const char* SETTING_CHANNEL_REQUIRES_STOP =
      "gameplay/abilities/channel_requires_stop_command_only";
  static constexpr const char* SETTING_CASTING_MODE =
      "gameplay/abilities/casting_mode";
};

#endif  // GDEXTENSION_GAME_SETTINGS_H
