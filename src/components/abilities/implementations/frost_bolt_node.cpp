#include "frost_bolt_node.hpp"
#include "../../../debug/debug_macros.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::String;
using godot::UtilityFunctions;

FrostBoltNode::FrostBoltNode() {
  // Set sensible defaults for frost bolt
  set_ability_name("Frost Bolt");
  set_description(
      "Cast a frost bolt with windup dealing damage and slowing target");
  set_cast_type(static_cast<int>(CastType::CAST_TIME));
  set_cast_time(0.7f);
  set_cast_point(0.5f);
  set_targeting_type(static_cast<int>(TargetingType::UNIT_TARGET));
  set_base_damage(40.0f);
  set_range(10.0f);
  set_cooldown(6.0f);
}

FrostBoltNode::~FrostBoltNode() = default;

void FrostBoltNode::_bind_methods() {
  // No custom properties for FrostBoltNode, inherits all from AbilityNode
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "position"),
                       &FrostBoltNode::execute);
  ClassDB::bind_method(D_METHOD("can_execute_on_target", "caster", "target"),
                       &FrostBoltNode::can_execute_on_target);
  ClassDB::bind_method(D_METHOD("calculate_damage", "caster", "target"),
                       &FrostBoltNode::calculate_damage);
}

bool FrostBoltNode::execute(Unit* caster,
                            Unit* target,
                            godot::Vector3 position) {
  if (caster == nullptr) {
    DBG_ERROR("FrostBolt", "No caster provided");
    return false;
  }

  if (target == nullptr) {
    DBG_ERROR("FrostBolt", "No target provided");
    return false;
  }

  if (!target->is_inside_tree()) {
    DBG_ERROR("FrostBolt", "Target is not in tree");
    return false;
  }

  // Calculate damage
  float damage = calculate_damage(caster, target);

  // Fire-and-forget: emit take_damage signal, don't wait for response
  caster->relay(take_damage, damage, target);
  DBG_INFO("FrostBolt", String(caster->get_name()) + " dealt " +
                            String::num(damage) + " damage to " +
                            String(target->get_name()));

  // TODO: Apply slow effect to target
  // For now, just damage. Slow effect can be added with a status effect system
  // later.
  return true;
}

bool FrostBoltNode::can_execute_on_target(Unit* caster, Unit* target) const {
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

float FrostBoltNode::calculate_damage(Unit* caster, Unit* target) const {
  // Simple damage: just return base damage
  // Subclasses can override to add scaling (e.g., based on caster stats)
  return get_base_damage();
}
