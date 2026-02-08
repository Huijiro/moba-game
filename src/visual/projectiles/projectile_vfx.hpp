#ifndef GDEXTENSION_PROJECTILE_VFX_H
#define GDEXTENSION_PROJECTILE_VFX_H

#include <godot_cpp/variant/vector3.hpp>

#include "../vfx_node.hpp"

using godot::Vector3;

/// Projectile VFX - Visual representation of a projectile
/// Follows the actual projectile node (SkillshotProjectile)
/// Automatically cleans up when projectile is destroyed
///
/// Parameters:
/// - "projectile": Node3D - The actual projectile to follow
///
/// Example:
/// projectile_vfx->play({
///   {"projectile", projectile_node}
/// });
///
/// The VFX will mirror the projectile's position and be destroyed
/// when the projectile is destroyed (on hit or max distance)
class ProjectileVFX : public VFXNode {
  GDCLASS(ProjectileVFX, VFXNode)

 protected:
  static void _bind_methods();

  // Reference to the projectile we're following
  godot::Object* tracked_projectile = nullptr;

 public:
  ProjectileVFX();
  ~ProjectileVFX();

  void _process(double delta) override;

  // Start following a projectile node
  void play(const Dictionary& params) override;
};

#endif  // GDEXTENSION_PROJECTILE_VFX_H
