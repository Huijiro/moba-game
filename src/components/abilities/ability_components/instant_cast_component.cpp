#include "instant_cast_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Ref;
using godot::RefCounted;

InstantCastComponent::InstantCastComponent() = default;

InstantCastComponent::~InstantCastComponent() = default;

void InstantCastComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_activated", "context"),
                       &InstantCastComponent::_on_activated);
}

void InstantCastComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("activated",
                     godot::Callable(this, "_on_activated"));
  }
}

void InstantCastComponent::_on_activated(const Ref<RefCounted>& context) {
  AbilityNode* ability = get_ability();
  if (ability == nullptr) {
    return;
  }

  DBG_INFO("InstantCastComponent", "Instant cast — firing immediately");
  ability->emit_signal("execute", context);
  ability->emit_signal("completed", context);
}
