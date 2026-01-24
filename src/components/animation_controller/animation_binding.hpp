/// @file animation_binding.hpp
/// @brief Data class for animation trigger bindings
///
/// Represents a single animation mapping: which signal triggers which animation
/// at what speed. Exposed as an inspectable Resource for editor configuration.

#ifndef GDEXTENSION_ANIMATION_BINDING_H
#define GDEXTENSION_ANIMATION_BINDING_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string_name.hpp>

using godot::Resource;

/// @class AnimationBinding
/// @brief Inspectable animation trigger binding
///
/// Defines: when this signal fires, play this animation at this speed.
/// Appears in Inspector as a resource property with three fields.
class AnimationBinding : public Resource {
  GDCLASS(AnimationBinding, Resource)

 protected:
  static void _bind_methods();

  /// Name of animation to play (e.g., "walk", "attack", "death")
  godot::StringName animation_name;

  /// Signal that triggers this animation (e.g.,
  /// "MovementComponent:movement_started") Empty = manual only (play via
  /// play_animation() call)
  godot::StringName trigger_signal;

  /// Speed multiplier for this animation (1.0 = normal, 1.2 = 20% faster)
  float playback_speed = 1.0f;

 public:
  AnimationBinding();
  ~AnimationBinding();

  /// Set animation name
  void set_animation_name(const godot::StringName& name);
  /// Get animation name
  godot::StringName get_animation_name() const;

  /// Set trigger signal path
  void set_trigger_signal(const godot::StringName& signal);
  /// Get trigger signal path
  godot::StringName get_trigger_signal() const;

  /// Set playback speed
  void set_playback_speed(float speed);
  /// Get playback speed
  float get_playback_speed() const;
};

#endif  // GDEXTENSION_ANIMATION_BINDING_H
