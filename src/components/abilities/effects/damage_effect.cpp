#include "damage_effect.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../core/unit.hpp"
#include "../../health/health_component.hpp"
#include "../ability_definition.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Object;
using godot::UtilityFunctions;

DamageEffect::DamageEffect() = default;

DamageEffect::~DamageEffect() = default;

void DamageEffect::_bind_methods() {
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "ability"),
                       &DamageEffect::execute);
}

void DamageEffect::execute(Unit* caster,
                           Object* target,
                           const AbilityDefinition* ability) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (target == nullptr || ability == nullptr) {
    UtilityFunctions::print("[DamageEffect] Invalid target or ability");
    return;
  }

  // Cast target to Unit
  Unit* target_unit = Object::cast_to<Unit>(target);
  if (target_unit == nullptr || !target_unit->is_inside_tree()) {
    UtilityFunctions::print(
        "[DamageEffect] Target is not a Unit or not in tree");
    return;
  }

  // Get health component from target
  HealthComponent* target_health = Object::cast_to<HealthComponent>(
      target_unit->get_component_by_class("HealthComponent"));

  if (target_health == nullptr) {
    UtilityFunctions::print("[DamageEffect] Target has no HealthComponent");
    return;
  }

  // Apply damage
  float damage = ability->get_base_damage();
  target_health->apply_damage(damage, caster);

  UtilityFunctions::print("[DamageEffect] Applied " +
                          godot::String::num(damage) + " damage to " +
                          target_unit->get_name());
}
