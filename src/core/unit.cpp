#include "unit.hpp"

#include "../components/abilities/ability_component.hpp"
#include "../components/combat/attack_component.hpp"
#include "../components/health/health_component.hpp"
#include "../components/interaction/interactable.hpp"
#include "../components/movement/movement_component.hpp"
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
  ClassDB::bind_method(D_METHOD("issue_interact_order", "target"),
                       &Unit::issue_interact_order);
  ClassDB::bind_method(D_METHOD("stop_order"), &Unit::stop_order);

  ClassDB::bind_method(D_METHOD("set_faction_id", "faction_id"),
                       &Unit::set_faction_id);
  ClassDB::bind_method(D_METHOD("get_faction_id"), &Unit::get_faction_id);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "faction_id"), "set_faction_id",
               "get_faction_id");

  ClassDB::bind_method(D_METHOD("set_unit_name", "name"), &Unit::set_unit_name);
  ClassDB::bind_method(D_METHOD("get_unit_name"), &Unit::get_unit_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "unit_name"), "set_unit_name",
               "get_unit_name");

  // Unit definition setup
  ADD_SIGNAL(MethodInfo("order_changed",
                        PropertyInfo(Variant::INT, "previous_order"),
                        PropertyInfo(Variant::INT, "new_order"),
                        PropertyInfo(Variant::OBJECT, "target")));
}

void Unit::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  // Unit doesn't need to do anything - components self-initialize
  // They can look up each other via get_component_by_class() as needed
}

void Unit::issue_move_order(const Vector3& position) {
  // Emit signal for components to listen to
  // MovementComponent will handle setting desired_location
  emit_signal("order_changed", static_cast<int>(OrderType::NONE),
              static_cast<int>(OrderType::MOVE), nullptr);

  MovementComponent* mov_comp = Object::cast_to<MovementComponent>(
      get_component_by_class("MovementComponent"));
  if (mov_comp != nullptr) {
    mov_comp->set_desired_location(position);
  }
}

void Unit::issue_attack_order(Unit* target) {
  // Emit signal for components to listen to
  emit_signal("order_changed", static_cast<int>(OrderType::NONE),
              static_cast<int>(OrderType::ATTACK), target);

  // Set target location for movement
  MovementComponent* mov_comp = Object::cast_to<MovementComponent>(
      get_component_by_class("MovementComponent"));
  if (mov_comp != nullptr && target != nullptr) {
    mov_comp->set_desired_location(target->get_global_position());
  }
}

void Unit::issue_chase_order(Unit* target) {
  // Emit signal for components to listen to
  emit_signal("order_changed", static_cast<int>(OrderType::NONE),
              static_cast<int>(OrderType::CHASE), target);

  // Set target location for movement
  MovementComponent* mov_comp = Object::cast_to<MovementComponent>(
      get_component_by_class("MovementComponent"));
  if (mov_comp != nullptr && target != nullptr) {
    mov_comp->set_desired_location(target->get_global_position());
  }
}

void Unit::issue_interact_order(Interactable* target) {
  // Emit signal for components to listen to
  emit_signal("order_changed", static_cast<int>(OrderType::NONE),
              static_cast<int>(OrderType::INTERACT), target);

  // Set target location for movement
  MovementComponent* mov_comp = Object::cast_to<MovementComponent>(
      get_component_by_class("MovementComponent"));
  if (mov_comp != nullptr && target != nullptr) {
    mov_comp->set_desired_location(target->get_global_position());
  }
}

void Unit::stop_order() {
  emit_signal("order_changed", static_cast<int>(OrderType::NONE),
              static_cast<int>(OrderType::NONE), nullptr);

  // Stop horizontal movement but keep vertical velocity (gravity)
  set_velocity(Vector3(0, get_velocity().y, 0));
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
  for (int i = 0; i < get_child_count(); ++i) {
    Node* child = get_child(i);
    auto* component = Object::cast_to<UnitComponent>(child);
    if (component) {
      component->register_debug_labels(registry);
    }
  }
}
