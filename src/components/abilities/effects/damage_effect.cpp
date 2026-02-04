#include "damage_effect.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
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
    DBG_INFO("DamageEffect", "Invalid target or ability");
    return;
  }

  // Cast target to Unit
  Unit* target_unit = Object::cast_to<Unit>(target);
  if (target_unit == nullptr || !target_unit->is_inside_tree()) {
    DBG_INFO("DamageEffect", "Target is not a Unit or not in tree");
    return;
  }

  // Apply damage via fire-and-forget signal
  float damage = ability->get_base_damage();
  caster->relay(take_damage, damage, target_unit);

  DBG_INFO("DamageEffect", "Applied " + godot::String::num(damage) +
                               " damage to " + target_unit->get_name());
}
