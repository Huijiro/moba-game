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
using godot::String;
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

  // Bind the register_signal method so components can call it
  ClassDB::bind_method(D_METHOD("register_signal", "signal_name"),
                       &Unit::register_signal);

  // No signals pre-registered - components will register the signals they need
  // in their _ready() methods
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

void Unit::register_signal(const StringName& signal_name) {
  // Use Godot's add_user_signal() which is the official way to dynamically
  // add signals to an object instance.
  // Multiple components might try to register the same signal, so we check
  // first to avoid "already existing signal" errors.

  // Check if signal already exists
  if (has_signal(signal_name)) {
    return;  // Signal already registered, nothing to do
  }

  // Convert StringName to String for add_user_signal
  String signal_str = String(signal_name);

  // Add the signal for the first time
  add_user_signal(signal_str);
}
