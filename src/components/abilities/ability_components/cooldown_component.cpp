#include "cooldown_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::Variant;

CooldownComponent::CooldownComponent() = default;

CooldownComponent::~CooldownComponent() = default;

void CooldownComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_cooldown", "duration"),
                       &CooldownComponent::set_cooldown);
  ClassDB::bind_method(D_METHOD("get_cooldown"),
                       &CooldownComponent::get_cooldown);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cooldown"),
               "set_cooldown", "get_cooldown");

  ClassDB::bind_method(D_METHOD("is_on_cooldown"),
                       &CooldownComponent::is_on_cooldown);
  ClassDB::bind_method(D_METHOD("get_remaining"),
                       &CooldownComponent::get_remaining);

  ClassDB::bind_method(D_METHOD("_on_validate", "context"),
                       &CooldownComponent::_on_validate);
  ClassDB::bind_method(D_METHOD("_on_completed", "context"),
                       &CooldownComponent::_on_completed);
}

void CooldownComponent::_ready() {
  AbilitySubcomponent::_ready();

  set_physics_process(false);

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("validate",
                     godot::Callable(this, "_on_validate"));
    ability->connect("completed",
                     godot::Callable(this, "_on_completed"));
  }
}

void CooldownComponent::_physics_process(double delta) {
  if (remaining <= 0.0f) {
    remaining = 0.0f;
    set_physics_process(false);

    AbilityNode* ability = get_ability();
    if (ability != nullptr) {
      // Create a minimal context for the signal
      Ref<AbilityContext> ctx;
      ctx.instantiate();
      ability->emit_signal("cooldown_finished", ctx);
    }
    return;
  }

  remaining -= delta;
}

void CooldownComponent::set_cooldown(float duration) { cooldown = duration; }
float CooldownComponent::get_cooldown() const { return cooldown; }

bool CooldownComponent::is_on_cooldown() const { return remaining > 0.0f; }
float CooldownComponent::get_remaining() const { return remaining; }

void CooldownComponent::_on_validate(const Ref<RefCounted>& context) {
  if (remaining > 0.0f) {
    Ref<AbilityContext> ctx = context;
    if (ctx.is_valid()) {
      ctx->block("on_cooldown");
    }
  }
}

void CooldownComponent::_on_completed(const Ref<RefCounted>& context) {
  remaining = cooldown;
  set_physics_process(true);

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    Ref<AbilityContext> ctx = context;
    ability->emit_signal("cooldown_started", ctx);
    DBG_INFO("CooldownComponent", "Cooldown started: " +
                 godot::String::num(cooldown, 1) + "s");
  }
}
