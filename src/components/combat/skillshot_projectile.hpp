#ifndef GDEXTENSION_SKILLSHOT_PROJECTILE_H
#define GDEXTENSION_SKILLSHOT_PROJECTILE_H

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Area3D;
using godot::Vector3;

class Unit;

/// Skillshot projectile — travels in a straight line, detects collision via Area3D.
///
/// Extends Area3D so Godot's physics handles overlap detection.
/// Add a CollisionShape3D child in the scene to define the hitbox.
///
/// Does NOT deal damage or apply effects. Emits signals:
///   hit(unit, position)          — overlapped with a Unit's body
///   reached_max_range(position)  — reached max distance without hitting
///
/// The ability system connects to these signals to decide what happens.
class SkillshotProjectile : public Area3D {
  GDCLASS(SkillshotProjectile, Area3D)

 protected:
  static void _bind_methods();

  Unit* caster = nullptr;
  float speed = 30.0f;
  float max_distance = 20.0f;
  float travel_distance = 0.0f;

  Vector3 direction = Vector3(0, 0, -1);

 public:
  SkillshotProjectile();
  ~SkillshotProjectile();

  void _ready() override;
  void _physics_process(double delta) override;

  /// Setup projectile with travel parameters.
  void setup(Unit* caster_unit,
             const Vector3& travel_direction,
             float travel_speed,
             float max_range);

  Unit* get_caster() const;

  void set_speed(float s);
  float get_speed() const;

  void set_max_distance(float distance);
  float get_max_distance() const;

 private:
  void _on_body_entered(godot::Node3D* body);
};

#endif  // GDEXTENSION_SKILLSHOT_PROJECTILE_H
