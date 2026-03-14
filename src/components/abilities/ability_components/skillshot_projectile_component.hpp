#ifndef SKILLSHOT_PROJECTILE_COMPONENT_HPP
#define SKILLSHOT_PROJECTILE_COMPONENT_HPP

#include <godot_cpp/classes/node3d.hpp>

#include "../ability_subcomponent.hpp"

class Unit;
class SkillshotProjectile;

/// Spawns a skillshot projectile that travels in a straight line.
/// The projectile template is a child of this component (first Node3D child).
/// On execute: duplicates the template, spawns it into the scene aimed at the
/// target position. Can miss — target can dodge. Detonates on first hit or max range.
///
/// The collision shape is defined in the projectile scene (CollisionShape3D child
/// of the SkillshotProjectile Area3D). No hit_radius needed here.
class SkillshotProjectileComponent : public AbilitySubcomponent {
  GDCLASS(SkillshotProjectileComponent, AbilitySubcomponent);

 public:
  SkillshotProjectileComponent();
  ~SkillshotProjectileComponent();

  void _ready() override;

  void set_damage(float d);
  float get_damage() const;

  void set_speed(float s);
  float get_speed() const;

  void set_max_distance(float d);
  float get_max_distance() const;

 protected:
  static void _bind_methods();

 private:
  float damage = 0.0f;
  float speed = 30.0f;
  float max_distance = 20.0f;

  void _on_execute(const godot::Ref<godot::RefCounted>& context);
  void _on_projectile_hit(godot::Object* unit, const godot::Vector3& position);
  void _on_projectile_max_range(const godot::Vector3& position);

  /// Find first Node3D child to use as projectile template
  godot::Node3D* _find_template() const;
};

#endif  // SKILLSHOT_PROJECTILE_COMPONENT_HPP
