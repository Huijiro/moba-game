#include "beam_node.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../core/unit.hpp"
#include "../../health/health_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::String;
using godot::UtilityFunctions;

BeamNode::BeamNode() {
  // Set sensible defaults for beam
  set_ability_name("Energy Beam");
  set_description("Channel beam dealing damage per tick while holding button");
  set_cast_type(static_cast<int>(CastType::CHANNEL));
  set_channel_duration(2.0f);
  set_channel_tick_interval(0.5f);
  set_targeting_type(static_cast<int>(TargetingType::UNIT_TARGET));
  set_base_damage(20.0f);
  set_range(10.0f);
  set_cooldown(6.0f);
}

BeamNode::~BeamNode() = default;

void BeamNode::_bind_methods() {
  // No custom properties for BeamNode, inherits all from AbilityNode
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "position"),
                       &BeamNode::execute);
  ClassDB::bind_method(D_METHOD("can_execute_on_target", "caster", "target"),
                       &BeamNode::can_execute_on_target);
  ClassDB::bind_method(D_METHOD("calculate_damage", "caster", "target"),
                       &BeamNode::calculate_damage);
}

void BeamNode::execute(Unit* caster, Unit* target, godot::Vector3 position) {
  if (caster == nullptr) {
    UtilityFunctions::print("[Beam] No caster provided");
    return;
  }

  if (target == nullptr) {
    UtilityFunctions::print("[Beam] No target provided");
    return;
  }

  if (!target->is_inside_tree()) {
    UtilityFunctions::print("[Beam] Target is not in tree");
    return;
  }

  // Get health component from target
  HealthComponent* target_health = Object::cast_to<HealthComponent>(
      target->get_component_by_class("HealthComponent"));

  if (target_health == nullptr) {
    UtilityFunctions::print("[Beam] Target has no HealthComponent");
    return;
  }

  // For channel abilities, this executes ONE TICK of the channel
  // The AbilityComponent is responsible for calling this repeatedly
  // at channel_tick_interval to simulate the channel

  float tick_damage = calculate_damage(caster, target);
  target_health->apply_damage(tick_damage, caster);

  UtilityFunctions::print("[Beam] " + caster->get_name() + " hit " +
                          target->get_name() + " for " +
                          String::num(tick_damage) + " damage (tick)");
}

bool BeamNode::can_execute_on_target(Unit* caster, Unit* target) const {
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

float BeamNode::calculate_damage(Unit* caster, Unit* target) const {
  // Return damage per tick
  // Note: total damage = base_damage * number_of_ticks
  // where number_of_ticks = channel_duration / channel_tick_interval
  return get_base_damage();
}
