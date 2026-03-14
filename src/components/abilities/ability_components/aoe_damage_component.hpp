#ifndef AOE_DAMAGE_COMPONENT_HPP
#define AOE_DAMAGE_COMPONENT_HPP

#include <godot_cpp/variant/typed_array.hpp>

#include "../ability_subcomponent.hpp"

class Unit;

/// Deals damage to all units within a radius of the target position.
/// Listens to execute. Uses context position as center.
class AoEDamageComponent : public AbilitySubcomponent {
  GDCLASS(AoEDamageComponent, AbilitySubcomponent);

 public:
  AoEDamageComponent();
  ~AoEDamageComponent();

  void _ready() override;

  void set_base_damage(float damage);
  float get_base_damage() const;

  void set_radius(float r);
  float get_radius() const;

  void set_damage_caster(bool damage);
  bool get_damage_caster() const;

 protected:
  static void _bind_methods();

 private:
  float base_damage = 0.0f;
  float radius = 5.0f;
  bool damage_caster = false;

  void _on_execute(const godot::Ref<godot::RefCounted>& context);
  void _find_and_damage_units(Unit* caster, const godot::Vector3& center);
  void _collect_units(godot::Node* node, godot::TypedArray<godot::Node>& out);
};

#endif  // AOE_DAMAGE_COMPONENT_HPP
