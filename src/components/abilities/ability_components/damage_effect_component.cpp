#include "damage_effect_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::String;
using godot::Variant;

DamageEffectComponent::DamageEffectComponent() = default;

DamageEffectComponent::~DamageEffectComponent() = default;

void DamageEffectComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_base_damage", "damage"),
                       &DamageEffectComponent::set_base_damage);
  ClassDB::bind_method(D_METHOD("get_base_damage"),
                       &DamageEffectComponent::get_base_damage);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_damage"),
               "set_base_damage", "get_base_damage");

  ClassDB::bind_method(D_METHOD("_on_execute", "context"),
                       &DamageEffectComponent::_on_execute);
  ClassDB::bind_method(D_METHOD("_on_tick", "context"),
                       &DamageEffectComponent::_on_tick);
}

void DamageEffectComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("execute",
                     godot::Callable(this, "_on_execute"));
    ability->connect("tick",
                     godot::Callable(this, "_on_tick"));
  }
}

void DamageEffectComponent::set_base_damage(float damage) {
  base_damage = damage;
}

float DamageEffectComponent::get_base_damage() const {
  return base_damage;
}

void DamageEffectComponent::_on_execute(
    const Ref<RefCounted>& context) {
  _apply_damage(context);
}

void DamageEffectComponent::_on_tick(
    const Ref<RefCounted>& context) {
  _apply_damage(context);
}

void DamageEffectComponent::_apply_damage(const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) {
    return;
  }

  Unit* target = ctx->get_target();
  if (target == nullptr) {
    DBG_DEBUG("DamageEffectComponent", "No target to damage");
    return;
  }

  if (base_damage <= 0.0f) {
    return;
  }

  // Apply damage through Unit's relay signal system
  Unit* caster = ctx->get_caster();
  target->relay(get_take_damage(), base_damage,
                caster != nullptr ? (godot::Object*)caster : nullptr);

  AbilityNode* ability = get_ability();
  String ability_name = ability ? ability->get_ability_name() : "Unknown";
  DBG_INFO("DamageEffectComponent",
           ability_name + " dealt " + String::num(base_damage, 1) +
               " damage to " + target->get_name());
}
