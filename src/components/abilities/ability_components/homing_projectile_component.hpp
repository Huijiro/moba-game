#ifndef HOMING_PROJECTILE_COMPONENT_HPP
#define HOMING_PROJECTILE_COMPONENT_HPP

#include <godot_cpp/classes/node3d.hpp>

#include "../ability_subcomponent.hpp"

class Unit;
class Projectile;

/// Spawns a homing projectile that tracks a unit target.
/// The projectile template is a child of this component (first Node3D child).
/// On execute: duplicates the template, spawns it into the scene tracking
/// the target. Always hits — cannot miss.
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

 protected:
  static void _bind_methods();

 private:
  float damage = 0.0f;
  float speed = 20.0f;

  void _on_execute(const godot::Ref<godot::RefCounted>& context);

  /// Find first Node3D child to use as projectile template
  godot::Node3D* _find_template() const;
};

#endif  // HOMING_PROJECTILE_COMPONENT_HPP
