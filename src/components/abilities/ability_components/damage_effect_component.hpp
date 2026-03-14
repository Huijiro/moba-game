#ifndef DAMAGE_EFFECT_COMPONENT_HPP
#define DAMAGE_EFFECT_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

/// Deals damage when the ability fires.
/// Listens to execute and tick to apply damage.
class DamageEffectComponent : public AbilitySubcomponent {
  GDCLASS(DamageEffectComponent, AbilitySubcomponent);

 public:
  DamageEffectComponent();
  ~DamageEffectComponent();

  void _ready() override;

  void set_base_damage(float damage);
  float get_base_damage() const;

 protected:
  static void _bind_methods();

 private:
  float base_damage = 0.0f;

  void _on_execute(const godot::Ref<godot::RefCounted>& context);
  void _on_tick(const godot::Ref<godot::RefCounted>& context);
  void _apply_damage(const godot::Ref<godot::RefCounted>& context);
};

#endif  // DAMAGE_EFFECT_COMPONENT_HPP
