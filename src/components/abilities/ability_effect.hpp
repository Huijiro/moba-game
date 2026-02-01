#ifndef GDEXTENSION_ABILITY_EFFECT_H
#define GDEXTENSION_ABILITY_EFFECT_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Array;
using godot::RefCounted;
using godot::Vector3;

class Unit;
class AbilityDefinition;

/// Base class for ability effects - subclass to create custom ability behaviors
/// What the ability *does* when it executes
///
/// Documentation: See docs/ability-system/01_ADDING_ABILITIES.md
/// - Step 1: Choose or Create an Effect
/// - Available Effects section (DamageEffect, AoEDamageEffect)
/// - Advanced: Custom Effects section
///
/// Built-in Subclasses:
/// - DamageEffect: Single-target damage (damage_amount property)
/// - AoEDamageEffect: Multi-target area damage (damage_amount + tree traversal)
///
/// To Create Custom Effect:
/// 1. Subclass AbilityEffect (C++ required)
/// 2. Override execute(Unit* caster, Object* target)
/// 3. Register with GDREGISTER_CLASS(YourEffect) in register_types.cpp
/// 4. Use as .tres resource in AbilityDefinition
///
/// Example: See GodotGame/resources/abilities/*Effect.tres
class AbilityEffect : public RefCounted {
  GDCLASS(AbilityEffect, RefCounted)

 protected:
  static void _bind_methods();

 public:
  AbilityEffect();
  ~AbilityEffect();

  // Main entry point - subclasses override to implement effect logic
  // caster: the unit that cast the ability
  // target: the target of the ability (unit, point, or null depending on
  // targeting_type) ability: the ability definition containing stats like
  // base_damage, etc.
  virtual void execute(Unit* caster,
                       Object* target,
                       const AbilityDefinition* ability);

  // Helper method to query all units within a sphere
  // Used by AoE abilities to find affected targets
  // Returns array of Unit pointers
  Array query_units_in_sphere(const Vector3& center,
                              float radius,
                              Unit* exclude_unit = nullptr) const;
};

#endif  // GDEXTENSION_ABILITY_EFFECT_H
