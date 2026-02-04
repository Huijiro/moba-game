#ifndef GDEXTENSION_GAME_SETTINGS_H
#define GDEXTENSION_GAME_SETTINGS_H

#include <godot_cpp/classes/node.hpp>

#include "../common/casting_mode.hpp"

using godot::Node;

/// Game settings manager using Godot's ProjectSettings
/// Provides centralized access to game configuration
/// Settings can be overridden per-player during networking
class GameSettings {
 public:
  // Channel ability settings
  static bool get_channel_requires_stop_command_only();
  static void set_channel_requires_stop_command_only(bool value);

  // Casting mode settings
  static int get_casting_mode();
  static void set_casting_mode(int mode);
  static CastingMode get_casting_mode_enum();

  // Register default settings with ProjectSettings
  static void register_settings();

 private:
  // Setting keys
  static constexpr const char* SETTING_CHANNEL_REQUIRES_STOP =
      "gameplay/abilities/channel_requires_stop_command_only";
  static constexpr const char* SETTING_CASTING_MODE =
      "gameplay/abilities/casting_mode";
};

#endif  // GDEXTENSION_GAME_SETTINGS_H
