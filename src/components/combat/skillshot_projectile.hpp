#ifndef GDEXTENSION_SKILLSHOT_PROJECTILE_H
#define GDEXTENSION_SKILLSHOT_PROJECTILE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Node3D;
using godot::Vector3;

class Unit;
class AbilityDefinition;

/// Skillshot projectile - travels in a direction and detonates on impact or
/// distance Used for directional abilities like Fireball
///
/// Features:
/// - Travels in a fixed direction
/// - Detonates on unit collision or max distance reached
/// - Applies AoE effect at impact point
/// - Handles multiple units in explosion radius
class SkillshotProjectile : public Node3D {
  GDCLASS(SkillshotProjectile, Node3D)

 protected:
  static void _bind_methods();

  Unit* caster = nullptr;
  float damage = 0.0f;
  float speed = 30.0f;
  float max_distance = 20.0f;    // Max range before detonating
  float hit_radius = 0.5f;       // Collision radius for unit detection
  float aoe_radius = 5.0f;       // Explosion radius
  float travel_distance = 0.0f;  // Distance traveled so far

  Vector3 direction = Vector3(0, 0, -1);  // Direction of travel

  // Called when projectile hits something
  // If hit_target is provided, only damage that unit (single-target hit)
  // Otherwise, find all units in aoe_radius and damage them
  void _detonate(Unit* hit_target = nullptr);

  // Find all units hit by explosion
  void _find_and_damage_units();

 public:
  SkillshotProjectile();
  ~SkillshotProjectile();

  void _physics_process(double delta) override;

  /// Setup projectile with caster, direction, and projectile parameters
  /// All ability-specific data is passed as individual parameters
  /// Does not depend on AbilityDefinition - works with any ability type
  void setup(Unit* caster_unit,
             const Vector3& travel_direction,
             float damage_amount,
             float travel_speed,
             float max_range,
             float explosion_radius,
             float collision_radius);

  void set_speed(float s);
  float get_speed() const;

  void set_max_distance(float distance);
  float get_max_distance() const;

  void set_aoe_radius(float radius);
  float get_aoe_radius() const;

  void set_hit_radius(float radius);
  float get_hit_radius() const;
};

#endif  // GDEXTENSION_SKILLSHOT_PROJECTILE_H
