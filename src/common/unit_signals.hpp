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

// Convenience aliases for backwards compatibility
#define move_requested get_move_requested()
#define attack_requested get_attack_requested()
#define chase_requested get_chase_requested()
#define interact_requested get_interact_requested()
#define stop_requested get_stop_requested()
#define take_damage get_take_damage()

#endif  // GDEXTENSION_UNIT_SIGNALS_H
