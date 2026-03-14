#ifndef GDEXTENSION_SKILLSHOT_PROJECTILE_H
#define GDEXTENSION_SKILLSHOT_PROJECTILE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Node3D;
using godot::Vector3;

class Unit;

/// Skillshot projectile — travels in a straight line and detects collision.
///
/// Does NOT deal damage or apply effects. Emits signals:
///   hit(unit, position)          — collided with a unit
///   reached_max_range(position)  — reached max distance without hitting
///
/// The ability system connects to these signals to decide what happens.
class SkillshotProjectile : public Node3D {
  GDCLASS(SkillshotProjectile, Node3D)

 protected:
  static void _bind_methods();

  Unit* caster = nullptr;
  float speed = 30.0f;
  float max_distance = 20.0f;
  float hit_radius = 0.5f;
  float travel_distance = 0.0f;

  Vector3 direction = Vector3(0, 0, -1);

 public:
  SkillshotProjectile();
  ~SkillshotProjectile();

  void _physics_process(double delta) override;

  /// Setup projectile with travel parameters only.
  void setup(Unit* caster_unit,
             const Vector3& travel_direction,
             float travel_speed,
             float max_range,
             float collision_radius);

  Unit* get_caster() const;

  void set_speed(float s);
  float get_speed() const;

  void set_max_distance(float distance);
  float get_max_distance() const;

  void set_hit_radius(float radius);
  float get_hit_radius() const;
};

#endif  // GDEXTENSION_SKILLSHOT_PROJECTILE_H
