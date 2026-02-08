#ifndef GDEXTENSION_UNIT_SIGNALS_H
#define GDEXTENSION_UNIT_SIGNALS_H

#include <godot_cpp/variant/string_name.hpp>

using godot::StringName;

// Movement signals - emitted by Unit relay from InputManager/AI
// Use functions to get signal names lazily (avoids static initialization
// issues)
inline const StringName& get_move_requested() {
  static StringName signal = StringName("move_requested");
  return signal;
}

inline const StringName& get_attack_requested() {
  static StringName signal = StringName("attack_requested");
  return signal;
}

inline const StringName& get_chase_requested() {
  static StringName signal = StringName("chase_requested");
  return signal;
}

inline const StringName& get_chase_to_range_requested() {
  static StringName signal = StringName("chase_to_range_requested");
  return signal;
}

inline const StringName& get_interact_requested() {
  static StringName signal = StringName("interact_requested");
  return signal;
}

inline const StringName& get_stop_requested() {
  static StringName signal = StringName("stop_requested");
  return signal;
}

// Damage signals - emitted by Unit relay from attack systems
inline const StringName& get_take_damage() {
  static StringName signal = StringName("take_damage");
  return signal;
}

// Chase signals - emitted by movement system
inline const StringName& get_chase_range_reached() {
  static StringName signal = StringName("chase_range_reached");
  return signal;
}

// Ability casting signals - emitted by InputManager/AI, listened by
// AbilityComponent
inline const StringName& get_cast_ability_unit_target() {
  static StringName signal = StringName("cast_ability_unit_target");
  return signal;
}

inline const StringName& get_cast_ability_point_target() {
  static StringName signal = StringName("cast_ability_point_target");
  return signal;
}

// Ability query signals - for UI and component initialization
inline const StringName& get_ability_icon_requested() {
  static StringName signal = StringName("ability_icon_requested");
  return signal;
}

inline const StringName& get_resource_pool_requested() {
  static StringName signal = StringName("resource_pool_requested");
  return signal;
}

// Cooldown signals - emitted by AbilityComponent, listened by UI
inline const StringName& get_ability_cooldown_started() {
  static StringName signal = StringName("ability_cooldown_started");
  return signal;
}

inline const StringName& get_ability_cooldown_tick() {
  static StringName signal = StringName("ability_cooldown_tick");
  return signal;
}

// Health update signal - emitted by HealthComponent, listened by UI
inline const StringName& get_health_changed() {
  static StringName signal = StringName("health_changed");
  return signal;
}

// Convenience aliases for backwards compatibility
#define move_requested get_move_requested()
#define attack_requested get_attack_requested()
#define chase_requested get_chase_requested()
#define chase_to_range_requested get_chase_to_range_requested()
#define interact_requested get_interact_requested()
#define stop_requested get_stop_requested()
#define take_damage get_take_damage()
#define chase_range_reached get_chase_range_reached()
#define cast_ability_unit_target get_cast_ability_unit_target()
#define cast_ability_point_target get_cast_ability_point_target()
#define ability_icon_requested get_ability_icon_requested()
#define resource_pool_requested get_resource_pool_requested()
#define ability_cooldown_started get_ability_cooldown_started()
#define ability_cooldown_tick get_ability_cooldown_tick()
#define health_changed get_health_changed()

#endif  // GDEXTENSION_UNIT_SIGNALS_H
