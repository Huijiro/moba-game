#ifndef GDEXTENSION_VFX_NODE_H
#define GDEXTENSION_VFX_NODE_H

#include <functional>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <map>

using godot::AnimationPlayer;
using godot::Array;
using godot::Dictionary;
using godot::Node3D;
using godot::String;

/// Base class for all VFX effects
/// VFX nodes are placed as children in ability scenes and triggered by
/// AbilityNode Uses Godot's Tween system for parameterized animations
///
/// Subclasses implement:
/// - play(params) - Start the VFX with custom parameters
/// - _on_finished() - Called when animation completes
///
/// Features:
/// - Auto-cleanup after animation finishes
/// - Tween-based animations for dynamic parameterization
/// - Finished signal for external tracking
class VFXNode : public Node3D {
  GDCLASS(VFXNode, Node3D)

 protected:
  static void _bind_methods();

  // Tween reference - keeps tween alive until completion
  // Using raw pointer to avoid issues with incomplete Tween types from
  // godot-cpp
  void* active_tween = nullptr;

  // Whether this VFX is currently playing
  bool is_playing_internal = false;

  // Duration of the effect (used for auto-cleanup timeout)
  float duration = 1.0f;

  // Timer for auto-cleanup
  float elapsed_time = 0.0f;

  // Animation-driven callback system
  std::map<String, std::function<void()>> callbacks;  // Signal name → callback
  Array expected_signals;  // Signals animation should emit
  AnimationPlayer* animation_player_cache = nullptr;  // Cached reference

  // Called when animation finishes - subclasses can override for cleanup
  virtual void _on_finished();

  // Animation signal callback system
  void _on_animation_signal(
      String signal_name);            // Receives signal from AnimationPlayer
  void validate_animation_signals();  // Warn if signals don't match
  AnimationPlayer* get_animation_player();  // Get/cache AnimationPlayer
  void clear_callbacks();                   // Clear all callbacks

 public:
  // Public callback registration - used by abilities to register callbacks
  void register_callback(String signal_name, std::function<void()> callback);
  VFXNode();
  ~VFXNode();

  void _process(double delta) override;

  // Main entry point - override in subclasses
  // params contains VFX-specific parameters like position, scale, intensity,
  // etc.
  virtual void play(const Dictionary& params);

  // Stop the current animation
  void stop();

  // Check if VFX is currently playing
  bool is_playing() const;

  // Set duration (for tween auto-cleanup)
  void set_duration(float seconds);
  float get_duration() const;

  // Subclasses can override play() to implement their animations
  // No direct tween support due to godot-cpp Ref<Tween> limitations
};

#endif  // GDEXTENSION_VFX_NODE_H
