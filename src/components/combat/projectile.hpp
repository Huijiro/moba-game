#ifndef GDEXTENSION_PROJECTILE_H
#define GDEXTENSION_PROJECTILE_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Area3D;
using godot::Vector3;

class Unit;

/// Homing projectile — tracks a unit target, detects collision via Area3D.
///
/// Extends Area3D so Godot's physics handles overlap detection.
/// Add a CollisionShape3D child in the scene to define the hitbox.
///
/// Applies damage directly on hit (unlike SkillshotProjectile which
/// just emits signals for the ability system to handle).
class Projectile : public Area3D {
  GDCLASS(Projectile, Area3D)

 protected:
  static void _bind_methods();

  Unit* attacker = nullptr;
  Unit* target = nullptr;
  float damage = 0.0f;
  float speed = 20.0f;

  Vector3 direction = Vector3(0, 0, 0);

 public:
  Projectile();
  ~Projectile();

  void _ready() override;
  void _physics_process(double delta) override;

  /// Setup projectile with attacker, target, damage, and speed
  void setup(Unit* attacker_unit,
             Unit* target_unit,
             float damage_amount,
             float travel_speed);

 private:
  void _on_body_entered(godot::Node3D* body);
};

#endif  // GDEXTENSION_PROJECTILE_H
