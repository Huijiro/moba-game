#ifndef GDEXTENSION_VISUAL_EFFECT_H
#define GDEXTENSION_VISUAL_EFFECT_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Node3D;
using godot::Vector3;

/// Base class for visual effects
/// Placeholders for actual visual implementations (particles, meshes, etc.)
/// Allows ability system to trigger visuals without being tightly coupled to
/// them
///
/// Subclasses can be created later to add:
/// - Particle effects
/// - Mesh animations
/// - Sound effects
/// - Shader effects
class VisualEffect : public Node3D {
  GDCLASS(VisualEffect, Node3D)

 protected:
  static void _bind_methods();

  // Override these in subclasses
  virtual void _play_at_position(const Vector3& position);
  virtual void _play_toward_direction(const Vector3& from,
                                      const Vector3& direction,
                                      float distance);
  virtual void _stop();

 public:
  VisualEffect();
  ~VisualEffect();

  // Public interface for ability system
  void play_at_position(const Vector3& position);
  void play_toward_direction(const Vector3& from,
                             const Vector3& direction,
                             float distance);
  void stop();

  // Timeline
  void set_duration(float seconds);
  float get_duration() const;

  bool is_playing() const;

 private:
  float duration = 1.0f;
  bool playing = false;
};

#endif  // GDEXTENSION_VISUAL_EFFECT_H
