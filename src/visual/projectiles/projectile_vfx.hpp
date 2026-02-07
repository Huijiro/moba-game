#ifndef GDEXTENSION_PROJECTILE_VFX_H
#define GDEXTENSION_PROJECTILE_VFX_H

#include <godot_cpp/variant/vector3.hpp>

#include "../vfx_node.hpp"

using godot::Vector3;

/// Projectile VFX - Animates a mesh/particle from start to end position
/// Used for directional abilities like Frost Bolt, Fireball projectiles
///
/// Parameters:
/// - "from": Vector3 - Starting position
/// - "to": Vector3 - Ending position
/// - "duration": float - Travel time in seconds
///
/// Example:
/// projectile_vfx->play({
///   {"from", caster_pos},
///   {"to", target_pos},
///   {"duration", 0.5f}
/// });
class ProjectileVFX : public VFXNode {
  GDCLASS(ProjectileVFX, VFXNode)

 protected:
  static void _bind_methods();

  // Store animation parameters
  Vector3 start_position = Vector3(0, 0, 0);
  Vector3 end_position = Vector3(0, 0, 0);
  float animation_duration = 1.0f;

 public:
  ProjectileVFX();
  ~ProjectileVFX();

  // Animate from start to end position
  void play(const Dictionary& params) override;
};

#endif  // GDEXTENSION_PROJECTILE_VFX_H
