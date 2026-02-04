#include "unit.hpp"

#include "../components/abilities/ability_component.hpp"
#include "../components/combat/attack_component.hpp"
#include "../components/health/health_component.hpp"
#include "../components/interaction/interactable.hpp"
#include "../components/ui/label_registry.hpp"
#include "../components/unit_component.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::MethodInfo;
using godot::Node;
using godot::PropertyInfo;
using godot::StringName;
using godot::Variant;
using godot::Vector3;

Unit::Unit() = default;

Unit::~Unit() = default;

void Unit::_bind_methods() {
  ClassDB::bind_method(D_METHOD("issue_move_order", "position"),
                       &Unit::issue_move_order);
  ClassDB::bind_method(D_METHOD("issue_attack_order", "target"),
                       &Unit::issue_attack_order);
  ClassDB::bind_method(D_METHOD("issue_chase_order", "target"),
                       &Unit::issue_chase_order);
  ClassDB::bind_method(D_METHOD("stop_order"), &Unit::stop_order);
  // Note: issue_interact_order is not bound because Interactable is an internal
  // C++ type not exposed to Godot. It can still be called from C++.

  ClassDB::bind_method(D_METHOD("set_faction_id", "faction_id"),
                       &Unit::set_faction_id);
  ClassDB::bind_method(D_METHOD("get_faction_id"), &Unit::get_faction_id);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "faction_id"), "set_faction_id",
               "get_faction_id");

  ClassDB::bind_method(D_METHOD("set_unit_name", "name"), &Unit::set_unit_name);
  ClassDB::bind_method(D_METHOD("get_unit_name"), &Unit::get_unit_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "unit_name"), "set_unit_name",
               "get_unit_name");

  // Signal emitted when unit receives an order - fire and forget
  // Components listen and independently decide what to do with it
  ADD_SIGNAL(MethodInfo("order_changed",
                        PropertyInfo(Variant::INT, "order_type"),
                        PropertyInfo(Variant::VECTOR3, "position_param"),
                        PropertyInfo(Variant::OBJECT, "target_param")));
}

void Unit::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  // Unit doesn't need to do anything - components self-initialize
  // They can look up each other via get_component_by_class() as needed
}

void Unit::issue_move_order(const Vector3& position) {
  // Emit order signal with destination position
  // MovementComponent and other components listen and respond independently
  emit_signal("order_changed", static_cast<int>(OrderType::MOVE), position,
              nullptr);
}

void Unit::issue_attack_order(Unit* target) {
  // Emit order signal with attack target
  // AttackComponent, MovementComponent, and others listen independently
  Vector3 target_pos =
      target != nullptr ? target->get_global_position() : Vector3();
  emit_signal("order_changed", static_cast<int>(OrderType::ATTACK), target_pos,
              target);
}

void Unit::issue_chase_order(Unit* target) {
  // Emit order signal with chase target
  // MovementComponent and AttackComponent listen independently
  Vector3 target_pos =
      target != nullptr ? target->get_global_position() : Vector3();
  emit_signal("order_changed", static_cast<int>(OrderType::CHASE), target_pos,
              target);
}

void Unit::issue_interact_order(Interactable* target) {
  // Emit order signal with interaction target
  // Components listen and decide how to respond
  Vector3 target_pos =
      target != nullptr ? target->get_global_position() : Vector3();
  emit_signal("order_changed", static_cast<int>(OrderType::INTERACT),
              target_pos, target);
}

void Unit::stop_order() {
  // Emit stop order - components independently handle stopping
  emit_signal("order_changed", static_cast<int>(OrderType::NONE), Vector3(),
              nullptr);
}

void Unit::set_faction_id(int32_t new_faction_id) {
  faction_id = new_faction_id;
}

int32_t Unit::get_faction_id() const {
  return faction_id;
}

Node* Unit::get_component_by_class(const StringName& class_name) const {
  const int32_t total_children = get_child_count();
  for (int32_t i = 0; i < total_children; ++i) {
    Node* child = get_child(i);
    if (child == nullptr) {
      continue;
    }
    if (child->get_class() == class_name) {
      return child;
    }
  }
  return nullptr;
}

HealthComponent* Unit::get_health_component() const {
  Node* component = get_component_by_class("HealthComponent");
  return Object::cast_to<HealthComponent>(component);
}

AttackComponent* Unit::get_attack_component() const {
  Node* component = get_component_by_class("AttackComponent");
  return Object::cast_to<AttackComponent>(component);
}

AbilityComponent* Unit::get_ability_component() const {
  Node* component = get_component_by_class("AbilityComponent");
  return Object::cast_to<AbilityComponent>(component);
}

void Unit::set_unit_name(const String& name) {
  unit_name = name;
}

String Unit::get_unit_name() const {
  return unit_name;
}

void Unit::register_all_debug_labels(LabelRegistry* registry) {
  if (!registry) {
    return;
  }

  // Register unit's own state
  registry->register_property("Unit", "name", unit_name);
  registry->register_property("Unit", "faction", String::num(faction_id));

  // Iterate all children and call register_debug_labels on any UnitComponents
  // Components track their own state (order, target, etc.)
  for (int i = 0; i < get_child_count(); ++i) {
    Node* child = get_child(i);
    auto* component = Object::cast_to<UnitComponent>(child);
    if (component) {
      component->register_debug_labels(registry);
    }
  }
}
