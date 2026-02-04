#include "instant_strike_node.hpp"
#include "../../../debug/debug_macros.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../core/unit.hpp"
#include "../../health/health_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::String;
using godot::UtilityFunctions;

InstantStrikeNode::InstantStrikeNode() {
  // Set sensible defaults for instant strike
  set_ability_name("Instant Strike");
  set_description("Instantly deal damage to target unit");
  set_cast_type(static_cast<int>(CastType::INSTANT));
  set_targeting_type(static_cast<int>(TargetingType::UNIT_TARGET));
  set_base_damage(30.0f);
  set_range(5.0f);
  set_cooldown(4.0f);
}

InstantStrikeNode::~InstantStrikeNode() = default;

void InstantStrikeNode::_bind_methods() {
  // No custom properties for InstantStrikeNode, inherits all from AbilityNode
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "position"),
                       &InstantStrikeNode::execute);
  ClassDB::bind_method(D_METHOD("can_execute_on_target", "caster", "target"),
                       &InstantStrikeNode::can_execute_on_target);
  ClassDB::bind_method(D_METHOD("calculate_damage", "caster", "target"),
                       &InstantStrikeNode::calculate_damage);
}

void InstantStrikeNode::execute(Unit* caster,
                                Unit* target,
                                godot::Vector3 position) {
  if (caster == nullptr) {
    DBG_INFO("InstantStrike", "No caster provided");
    return;
  }

  if (target == nullptr) {
    DBG_INFO("InstantStrike", "No target provided");
    return;
  }

  if (!target->is_inside_tree()) {
    DBG_INFO("InstantStrike", "Target is not in tree");
    return;
  }

  // Get health component from target
  HealthComponent* target_health = Object::cast_to<HealthComponent>(
      target->get_component_by_class("HealthComponent"));

  if (target_health == nullptr) {
    DBG_INFO("InstantStrike", "Target has no HealthComponent");
    return;
  }

  // Calculate and apply damage
  float damage = calculate_damage(caster, target);
  target_health->apply_damage(damage, caster);

  DBG_INFO("InstantStrike", String(caster->get_name()) + " dealt " +
                          String::num(damage) + " damage to " +
                          target->get_name());
}

bool InstantStrikeNode::can_execute_on_target(Unit* caster,
                                              Unit* target) const {
  if (caster == nullptr || target == nullptr) {
    return false;
  }

  // Can't target self
  if (caster == target) {
    return false;
  }

  // Check if target is in range
  float distance =
      caster->get_global_position().distance_to(target->get_global_position());
  if (distance > get_range()) {
    return false;
  }

  return true;
}

float InstantStrikeNode::calculate_damage(Unit* caster, Unit* target) const {
  // Simple damage: just return base damage
  // Subclasses can override to add scaling (e.g., based on caster stats)
  return get_base_damage();
}
