#ifndef HOMING_PROJECTILE_COMPONENT_HPP
#define HOMING_PROJECTILE_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

class Unit;
class Projectile;

/// Spawns a homing projectile that tracks a unit target.
/// On execute: duplicates the projectile child node from the
/// AbilityNode, spawns it into the scene, and sets it up to track the target.
/// Always hits — cannot miss.
class HomingProjectileComponent : public AbilitySubcomponent {
  GDCLASS(HomingProjectileComponent, AbilitySubcomponent);

 public:
  HomingProjectileComponent();
  ~HomingProjectileComponent();

  void _ready() override;

  void set_damage(float d);
  float get_damage() const;

  void set_speed(float s);
  float get_speed() const;

  void set_projectile_node_name(const godot::String& name);
  godot::String get_projectile_node_name() const;

 protected:
  static void _bind_methods();

 private:
  float damage = 0.0f;
  float speed = 20.0f;
  godot::String projectile_node_name = "projectile";

  void _on_execute(const godot::Ref<godot::RefCounted>& context);
};

#endif  // HOMING_PROJECTILE_COMPONENT_HPP
