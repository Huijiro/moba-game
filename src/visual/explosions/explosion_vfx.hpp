#ifndef GDEXTENSION_EXPLOSION_VFX_H
#define GDEXTENSION_EXPLOSION_VFX_H

#include <godot_cpp/variant/vector3.hpp>

#include "../vfx_node.hpp"

using godot::Vector3;

/// Explosion VFX - Burst effect at a point with customizable scale and
/// intensity Used for ability impacts like Fireball explosion, Frost Bolt
/// impact
///
/// Parameters:
/// - "position": Vector3 - Center of explosion
/// - "scale": float - Size multiplier (default 1.0)
/// - "intensity": float - Effect intensity (0.0-1.0, affects alpha)
/// - "duration": float - Effect duration in seconds
///
/// Example:
/// explosion_vfx->play({
///   {"position", impact_pos},
///   {"scale", 1.5f},
///   {"intensity", 0.8f},
///   {"duration", 0.6f}
/// });
class ExplosionVFX : public VFXNode {
  GDCLASS(ExplosionVFX, VFXNode)

 protected:
  static void _bind_methods();

  // Animation parameters
  Vector3 explosion_position = Vector3(0, 0, 0);
  float explosion_scale = 1.0f;
  float explosion_intensity = 1.0f;
  float effect_duration = 1.0f;

  // Validate that mesh size doesn't exceed hitbox
  void _validate_mesh_size();

 public:
  ExplosionVFX();
  ~ExplosionVFX();

  // Play explosion with scale/intensity parameters
  void play(const Dictionary& params) override;
};

#endif  // GDEXTENSION_EXPLOSION_VFX_H
