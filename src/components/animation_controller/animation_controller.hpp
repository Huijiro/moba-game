/// @file animation_controller.hpp
/// @brief Inspector-configurable animation playback controller
///
/// AnimationController manages animations through an inspector-exposed list
/// of animation bindings. Each binding maps a signal trigger to an animation
/// with a configurable playback speed.
///
/// Features:
///   - Inspector-friendly animation list configuration
///   - Automatic signal connection on _ready()
///   - Per-animation speed control
///   - Flexible signal parameter handling (supports 0 and 1 parameter)
///
/// Usage:
///   1. Add AnimationController as child of Unit
///   2. Unit must have AnimationPlayer child (searched recursively)
///   3. Configure animations in inspector:
///      - Add animation bindings (name, signal, speed)
///      - Signals are auto-connected to play_animation()
///   4. Done! No editor signal connections needed
///
/// Example Animation List:
///   - idle: no signal (plays on demand), speed 1.0
///   - walk: MovementComponent.movement_started, speed 1.0
///   - attack: AttackComponent.attack_started, speed 1.2
///   - death: HealthComponent.died, speed 0.5

#ifndef GDEXTENSION_ANIMATION_CONTROLLER_H
#define GDEXTENSION_ANIMATION_CONTROLLER_H

#include <godot_cpp/classes/animation_player.hpp>
#include <vector>

#include "animation_binding.hpp"
#include "unit_component.hpp"

using godot::AnimationPlayer;

/// @class AnimationController
/// @brief Inspector-configurable animation controller
///
/// Manages a list of animation bindings configured in the inspector.
/// Automatically connects signals and plays animations on demand.
class AnimationController : public UnitComponent {
  GDCLASS(AnimationController, UnitComponent)

 protected:
  static void _bind_methods();

  /// Cached reference to AnimationPlayer
  AnimationPlayer* animation_player = nullptr;

  /// List of animation bindings (inspector-configurable)
  godot::TypedArray<AnimationBinding> animation_bindings;

  /// Recursively search for first AnimationPlayer in tree
  AnimationPlayer* find_animation_player(godot::Node* node);

  /// Extract signal object from string path
  /// @param signal_path Full path to signal (e.g.,
  /// "../MovementComponent:movement_started")
  /// @return Signal object if found, nullptr otherwise
  godot::Object* find_signal_emitter(const godot::StringName& signal_path);

  /// Connect all signals in animation_bindings
  void connect_animation_signals();

 public:
  AnimationController();
  ~AnimationController();

  /// Initialize: find AnimationPlayer and connect signals
  void _ready() override;

  /// Set animation bindings list (called from inspector)
  /// @param bindings TypedArray of AnimationBinding objects
  void set_animations(const godot::TypedArray<AnimationBinding>& bindings);

  /// Get animation bindings list
  godot::TypedArray<AnimationBinding> get_animations() const;

  /// Play animation by name with optional speed override
  /// @param name Animation name to play
  /// @param speed Speed multiplier (if 0, uses configured speed)
  void play_animation(const godot::StringName& name, float speed = 0.0f);

  /// Play animation by index in bindings list
  /// @param index Index in animation_bindings
  /// @param speed Speed override (if 0, uses configured speed)
  void play_animation_index(int32_t index, float speed = 0.0f);
};

#endif  // GDEXTENSION_ANIMATION_CONTROLLER_H
