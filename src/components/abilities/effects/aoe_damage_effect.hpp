#ifndef GDEXTENSION_AOE_DAMAGE_EFFECT_H
#define GDEXTENSION_AOE_DAMAGE_EFFECT_H

#include <godot_cpp/variant/vector3.hpp>

#include "../ability_effect.hpp"

using godot::Vector3;

// Area of effect damage - affects all units within a radius
// Useful for abilities like Fireball, Meteor Strike, etc.
class AoEDamageEffect : public AbilityEffect {
  GDCLASS(AoEDamageEffect, AbilityEffect)

 protected:
  static void _bind_methods();

 public:
  AoEDamageEffect();
  ~AoEDamageEffect();

  // Execute AoE damage around a point
  // target can be a Unit (use its position) or null (use external point)
  // For point-target abilities, the point should be stored externally
  void execute(Unit* caster,
               Object* target,
               const AbilityDefinition* ability) override;

  // Position-based execution for point-target abilities
  // Damages all units around the specified point
  void execute_at_point(Unit* caster,
                        const Vector3& point,
                        const AbilityDefinition* ability) override;

 private:
  // Helper: Find all units in a sphere and apply damage
  void _apply_damage_in_radius(Unit* caster,
                               const Vector3& center,
                               float radius,
                               float damage);
};

#endif  // GDEXTENSION_AOE_DAMAGE_EFFECT_H
