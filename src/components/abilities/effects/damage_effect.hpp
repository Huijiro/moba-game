#ifndef GDEXTENSION_DAMAGE_EFFECT_H
#define GDEXTENSION_DAMAGE_EFFECT_H

#include "../ability_effect.hpp"

// Simple single-target damage effect
// Applies base_damage from ability definition to a single target
class DamageEffect : public AbilityEffect {
  GDCLASS(DamageEffect, AbilityEffect)

 protected:
  static void _bind_methods();

 public:
  DamageEffect();
  ~DamageEffect();

  // Execute damage effect against a target
  void execute(Unit* caster,
               Object* target,
               const AbilityDefinition* ability) override;
};

#endif  // GDEXTENSION_DAMAGE_EFFECT_H
