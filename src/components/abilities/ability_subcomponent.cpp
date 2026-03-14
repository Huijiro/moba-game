#include "ability_subcomponent.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "ability_node.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Object;
using godot::String;
using godot::UtilityFunctions;

AbilitySubcomponent::AbilitySubcomponent() = default;

AbilitySubcomponent::~AbilitySubcomponent() = default;

void AbilitySubcomponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_ability"),
                       &AbilitySubcomponent::get_ability);
}

void AbilitySubcomponent::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Validate parent is an AbilityNode
  Node* parent = get_parent();
  owner_ability = Object::cast_to<AbilityNode>(parent);

  if (owner_ability == nullptr) {
    UtilityFunctions::push_error(
        "[" + get_class() + "] must be a child of AbilityNode, but parent is: " +
        (parent != nullptr ? parent->get_class() : String("null")));
  }
}

AbilityNode* AbilitySubcomponent::get_ability() const {
  return owner_ability;
}
