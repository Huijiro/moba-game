#include "chase_into_range_component.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../ability_node.hpp"
#include "unit_targeting_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;

ChaseIntoRangeComponent::ChaseIntoRangeComponent() = default;

ChaseIntoRangeComponent::~ChaseIntoRangeComponent() = default;

void ChaseIntoRangeComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("validate_target_in_range", "caster", "target"),
                       &ChaseIntoRangeComponent::validate_target_in_range);
}

bool ChaseIntoRangeComponent::validate_target_in_range(Unit* caster,
                                                       Unit* target) const {
  if (caster == nullptr || target == nullptr) {
    return false;
  }

  // Get the parent ability
  AbilityNode* ability = get_ability();
  if (ability == nullptr) {
    DBG_WARN("ChaseIntoRangeComponent", "No parent ability found");
    return false;
  }

  // Find UnitTargetingComponent sibling to get the range
  UnitTargetingComponent* targeting = nullptr;
  for (int i = 0; i < ability->get_child_count(); i++) {
    Node* child = ability->get_child(i);
    targeting = Object::cast_to<UnitTargetingComponent>(child);
    if (targeting != nullptr) {
      break;
    }
  }

  if (targeting == nullptr) {
    DBG_WARN("ChaseIntoRangeComponent",
             "No UnitTargetingComponent sibling found");
    return false;
  }

  // Check if target is in range
  float range = targeting->get_range();
  float distance =
      caster->get_global_position().distance_to(target->get_global_position());

  return distance <= range;
}
