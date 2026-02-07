#ifndef GDEXTENSION_VFX_NODE_H
#define GDEXTENSION_VFX_NODE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using godot::Dictionary;
using godot::Node3D;
using godot::Ref;
using godot::Tween;

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
  Ref<Tween> active_tween = nullptr;

  // Whether this VFX is currently playing
  bool is_playing_internal = false;

  // Duration of the effect (used for auto-cleanup timeout)
  float duration = 1.0f;

  // Called when tween finishes - subclasses can override for cleanup
  virtual void _on_finished();

  // Create a tween for this VFX
  Ref<Tween> _create_tween();

 public:
  VFXNode();
  ~VFXNode();

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

  // Utility: Create a tween bound to this node
  // Tween will automatically stop if node is freed
  Ref<Tween> create_parameterized_tween();
};

#endif  // GDEXTENSION_VFX_NODE_H
