#include "instant_strike_node.hpp"
#include "../../../debug/debug_macros.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../ability_api.hpp"

using godot::ClassDB;
using godot::D_METHOD;
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

bool InstantStrikeNode::execute(Unit* caster,
                                Unit* target,
                                godot::Vector3 position) {
  if (caster == nullptr) {
    DBG_INFO("InstantStrike", "No caster provided");
    return false;
  }

  if (target == nullptr) {
    DBG_INFO("InstantStrike", "No target provided");
    return false;
  }

  if (!target->is_inside_tree()) {
    DBG_INFO("InstantStrike", "Target is not in tree");
    return false;
  }

  // Chase target if out of range
  bool ready_to_execute =
      AbilityAPI::chase_and_prepare_execution(caster, target, get_range());

  if (!ready_to_execute) {
    DBG_INFO("InstantStrike",
             String(caster->get_name()) + " chasing " + target->get_name());
    return false;  // Deferred execution
  }

  // Calculate damage
  float damage = calculate_damage(caster, target);

  // Fire-and-forget: emit take_damage signal, don't wait for response
  caster->relay(take_damage, damage, target);

  DBG_INFO("InstantStrike", String(caster->get_name()) + " dealt " +
                                String::num(damage) + " damage to " +
                                target->get_name());
  return true;  // Successfully executed
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

  // Don't check range here - let execute() handle chasing
  // This allows out-of-range targets to be accepted and chased
  return true;
}

float InstantStrikeNode::calculate_damage(Unit* caster, Unit* target) const {
  // Simple damage: just return base damage
  // Subclasses can override to add scaling (e.g., based on caster stats)
  return get_base_damage();
}
