#ifndef SKILLSHOT_PROJECTILE_COMPONENT_HPP
#define SKILLSHOT_PROJECTILE_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

class Unit;
class SkillshotProjectile;

/// Spawns a skillshot projectile that travels in a straight line.
/// On execute: duplicates the projectile child node from the
/// AbilityNode, spawns it into the scene aimed at the target position.
/// Can miss — target can dodge. Detonates on first hit or max range.
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

  void set_hit_radius(float r);
  float get_hit_radius() const;

  void set_projectile_node_name(const godot::String& name);
  godot::String get_projectile_node_name() const;

 protected:
  static void _bind_methods();

 private:
  float damage = 0.0f;
  float speed = 30.0f;
  float max_distance = 20.0f;
  float hit_radius = 0.5f;
  godot::String projectile_node_name = "projectile";

  void _on_execute(const godot::Ref<godot::RefCounted>& context);
  void _on_projectile_hit(godot::Object* unit, const godot::Vector3& position);
  void _on_projectile_max_range(const godot::Vector3& position);
};

#endif  // SKILLSHOT_PROJECTILE_COMPONENT_HPP
