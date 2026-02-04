#include "unit.hpp"

#include "../components/abilities/ability_component.hpp"
#include "../components/ui/label_registry.hpp"
#include "../components/unit_component.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/variant.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::MethodInfo;
using godot::Node;
using godot::PropertyInfo;
using godot::Variant;

Unit::Unit() = default;

Unit::~Unit() = default;

void Unit::_bind_methods() {
  // Metadata properties
  ClassDB::bind_method(D_METHOD("set_faction_id", "faction_id"),
                       &Unit::set_faction_id);
  ClassDB::bind_method(D_METHOD("get_faction_id"), &Unit::get_faction_id);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "faction_id"), "set_faction_id",
               "get_faction_id");

  ClassDB::bind_method(D_METHOD("set_unit_name", "name"), &Unit::set_unit_name);
  ClassDB::bind_method(D_METHOD("get_unit_name"), &Unit::get_unit_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "unit_name"), "set_unit_name",
               "get_unit_name");

  // Register relay signals - Unit is a transparent relay hub
  // Movement signals (InputManager/AI → Components)
  ADD_SIGNAL(
      MethodInfo("move_requested", PropertyInfo(Variant::VECTOR3, "target")));
  ADD_SIGNAL(MethodInfo("attack_requested",
                        PropertyInfo(Variant::OBJECT, "target"),
                        PropertyInfo(Variant::VECTOR3, "position")));
  ADD_SIGNAL(MethodInfo("chase_requested",
                        PropertyInfo(Variant::OBJECT, "target"),
                        PropertyInfo(Variant::VECTOR3, "position")));
  ADD_SIGNAL(MethodInfo("interact_requested",
                        PropertyInfo(Variant::OBJECT, "target"),
                        PropertyInfo(Variant::VECTOR3, "position")));
  ADD_SIGNAL(MethodInfo("stop_requested"));

  // Damage/Combat signals (Combat systems → Components)
  ADD_SIGNAL(MethodInfo("take_damage", PropertyInfo(Variant::FLOAT, "damage"),
                        PropertyInfo(Variant::OBJECT, "source")));
}

void Unit::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
}

void Unit::set_faction_id(int32_t new_faction_id) {
  faction_id = new_faction_id;
}

int32_t Unit::get_faction_id() const {
  return faction_id;
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

Node* Unit::get_component_by_class(const StringName& class_name) const {
  // TODO: Remove this method after refactoring ability system to use relay()
  // Temporary implementation to support abilities that need component lookups
  for (int i = 0; i < get_child_count(); ++i) {
    Node* child = get_child(i);
    if (child != nullptr && child->get_class() == class_name) {
      return child;
    }
  }
  return nullptr;
}

AbilityComponent* Unit::get_ability_component() const {
  // TODO: Refactor ability system to use relay() instead of direct lookups
  // Temporary implementation to support existing InputManager code
  Node* component = get_component_by_class("AbilityComponent");
  return Object::cast_to<AbilityComponent>(component);
}
