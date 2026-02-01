#include "ability_component.hpp"

#include <algorithm>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../core/unit.hpp"
#include "../resources/resource_pool_component.hpp"
#include "ability_definition.hpp"
#include "ability_effect.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::String;
using godot::UtilityFunctions;
using godot::Variant;

AbilityComponent::AbilityComponent() = default;

AbilityComponent::~AbilityComponent() = default;

void AbilityComponent::_bind_methods() {
  // ========== ABILITY SLOT METHODS ==========
  ClassDB::bind_method(D_METHOD("set_ability", "slot", "ability"),
                       &AbilityComponent::set_ability);
  ClassDB::bind_method(D_METHOD("get_ability", "slot"),
                       &AbilityComponent::get_ability);
  ClassDB::bind_method(D_METHOD("has_ability", "slot"),
                       &AbilityComponent::has_ability);

  // ========== CASTING METHODS ==========
  ClassDB::bind_method(D_METHOD("try_cast", "slot", "target"),
                       &AbilityComponent::try_cast);
  ClassDB::bind_method(D_METHOD("try_cast_point", "slot", "point"),
                       &AbilityComponent::try_cast_point);

  // ========== STATE QUERY METHODS ==========
  ClassDB::bind_method(D_METHOD("is_casting"), &AbilityComponent::is_casting);
  ClassDB::bind_method(D_METHOD("is_on_cooldown", "slot"),
                       &AbilityComponent::is_on_cooldown);
  ClassDB::bind_method(D_METHOD("get_cooldown_remaining", "slot"),
                       &AbilityComponent::get_cooldown_remaining);
  ClassDB::bind_method(D_METHOD("get_cooldown_duration", "slot"),
                       &AbilityComponent::get_cooldown_duration);
  ClassDB::bind_method(D_METHOD("get_cast_state", "slot"),
                       &AbilityComponent::get_cast_state);

  // NOTE: Abilities are set automatically from UnitDefinition in Unit._ready()
  // No ADD_PROPERTY needed here since set_ability() takes 2 parameters (slot,
  // ability) and Godot properties only support 1-parameter setters

  // ========== SIGNALS ==========
  ADD_SIGNAL(godot::MethodInfo("ability_cast_started",
                               PropertyInfo(Variant::INT, "slot"),
                               PropertyInfo(Variant::OBJECT, "target")));
  ADD_SIGNAL(godot::MethodInfo("ability_cast_point_reached",
                               PropertyInfo(Variant::INT, "slot"),
                               PropertyInfo(Variant::OBJECT, "target")));
  ADD_SIGNAL(godot::MethodInfo("ability_executed",
                               PropertyInfo(Variant::INT, "slot"),
                               PropertyInfo(Variant::OBJECT, "target")));
  ADD_SIGNAL(godot::MethodInfo("ability_cooldown_started",
                               PropertyInfo(Variant::INT, "slot"),
                               PropertyInfo(Variant::FLOAT, "duration")));
  ADD_SIGNAL(godot::MethodInfo("cast_failed",
                               PropertyInfo(Variant::INT, "slot"),
                               PropertyInfo(Variant::STRING, "reason")));
}

void AbilityComponent::_ready() {
  UnitComponent::_ready();

  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    UtilityFunctions::print("[AbilityComponent] No Unit owner found");
    return;
  }

  // Try to get resource pool for mana checks
  resource_pool = Object::cast_to<ResourcePoolComponent>(
      owner->get_component_by_class("ResourcePoolComponent"));

  if (resource_pool == nullptr) {
    UtilityFunctions::print(
        "[AbilityComponent] Warning: No ResourcePoolComponent for mana "
        "tracking");
  }
}

void AbilityComponent::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Update cooldown timers
  for (int i = 0; i < 4; i++) {
    if (cooldown_timers[i] > 0.0f) {
      cooldown_timers[i] -= delta;
      if (cooldown_timers[i] < 0.0f) {
        cooldown_timers[i] = 0.0f;
      }
    }
  }

  // Handle casting state machine
  if (casting_slot >= 0 && casting_slot < 4) {
    Ref<AbilityDefinition> ability = ability_slots[casting_slot];
    if (ability == nullptr) {
      _finish_casting();
      return;
    }

    // Advance casting timer
    casting_timer += delta;

    // Get cast point timing
    float cast_duration = 0.0f;
    int cast_type = ability->get_cast_type();

    if (cast_type == static_cast<int>(CastType::CAST_TIME)) {
      cast_duration = ability->get_cast_time();
    } else if (cast_type == static_cast<int>(CastType::CHANNEL)) {
      cast_duration = ability->get_channel_duration();
    }

    // Check if we've reached the cast point
    if (cast_duration > 0.0f) {
      float cast_point_time = cast_duration * ability->get_cast_point();

      if (casting_timer >= cast_point_time &&
          casting_state == static_cast<int>(CastState::CASTING)) {
        // Fire the ability at cast point
        emit_signal("ability_cast_point_reached", casting_slot, casting_target);
        _execute_ability(casting_slot);
        casting_state = static_cast<int>(CastState::ON_COOLDOWN);
      }
    } else {
      // Instant cast - execute immediately
      if (casting_state == static_cast<int>(CastState::CASTING)) {
        emit_signal("ability_cast_point_reached", casting_slot, casting_target);
        _execute_ability(casting_slot);
        casting_state = static_cast<int>(CastState::ON_COOLDOWN);
      }
    }

    // Check if casting is finished
    if (casting_timer >= cast_duration && cast_duration > 0.0f) {
      _finish_casting();
    }
  }
}

// ========== ABILITY SLOT MANAGEMENT ==========

void AbilityComponent::set_ability(int slot,
                                   const Ref<AbilityDefinition>& ability) {
  if (slot < 0 || slot >= 4) {
    UtilityFunctions::print("[AbilityComponent] Invalid slot: " +
                            String::num(slot));
    return;
  }
  ability_slots[slot] = ability;
}

Ref<AbilityDefinition> AbilityComponent::get_ability(int slot) {
  if (slot < 0 || slot >= 4) {
    return nullptr;
  }
  return ability_slots[slot];
}

bool AbilityComponent::has_ability(int slot) {
  if (slot < 0 || slot >= 4) {
    return false;
  }
  return ability_slots[slot] != nullptr;
}

// ========== ABILITY CASTING ==========

bool AbilityComponent::try_cast(int slot, Object* target) {
  if (!_can_cast(slot)) {
    emit_signal("cast_failed", slot, "not_ready");
    return false;
  }

  if (!_can_afford(slot)) {
    emit_signal("cast_failed", slot, "insufficient_resources");
    return false;
  }

  _begin_cast(slot, target);
  return true;
}

bool AbilityComponent::try_cast_point(int slot, const Vector3& point) {
  if (!_can_cast(slot)) {
    emit_signal("cast_failed", slot, "not_ready");
    return false;
  }

  if (!_can_afford(slot)) {
    emit_signal("cast_failed", slot, "insufficient_resources");
    return false;
  }

  // Store the point for the effect to use
  casting_point = point;
  _begin_cast(slot, nullptr);
  return true;
}

// ========== STATE QUERIES ==========

bool AbilityComponent::is_casting() const {
  return casting_slot >= 0 && casting_slot < 4 &&
         casting_state == static_cast<int>(CastState::CASTING);
}

bool AbilityComponent::is_on_cooldown(int slot) const {
  if (slot < 0 || slot >= 4) {
    return false;
  }
  return cooldown_timers[slot] > 0.0f;
}

float AbilityComponent::get_cooldown_remaining(int slot) const {
  if (slot < 0 || slot >= 4) {
    return 0.0f;
  }
  return cooldown_timers[slot];
}

float AbilityComponent::get_cooldown_duration(int slot) const {
  if (slot < 0 || slot >= 4) {
    return 0.0f;
  }
  Ref<AbilityDefinition> ability = ability_slots[slot];
  if (ability == nullptr) {
    return 0.0f;
  }
  return ability->get_cooldown();
}

int AbilityComponent::get_cast_state(int slot) const {
  if (slot < 0 || slot >= 4) {
    return static_cast<int>(CastState::IDLE);
  }
  if (slot == casting_slot) {
    return casting_state;
  }
  if (is_on_cooldown(slot)) {
    return static_cast<int>(CastState::ON_COOLDOWN);
  }
  return static_cast<int>(CastState::IDLE);
}

// ========== INTERNAL METHODS ==========

bool AbilityComponent::_can_cast(int slot) {
  if (slot < 0 || slot >= 4) {
    return false;
  }

  // Check if already casting
  if (is_casting()) {
    return false;
  }

  // Check if ability exists
  if (ability_slots[slot] == nullptr) {
    return false;
  }

  // Check if off cooldown
  if (is_on_cooldown(slot)) {
    return false;
  }

  return true;
}

bool AbilityComponent::_can_afford(int slot) {
  if (slot < 0 || slot >= 4) {
    return false;
  }

  Ref<AbilityDefinition> ability = ability_slots[slot];
  if (ability == nullptr) {
    return false;
  }

  // Check mana cost if we have a resource pool
  if (resource_pool != nullptr) {
    float mana_cost = ability->get_mana_cost();
    if (mana_cost > 0.0f) {
      // For now, assume pool has "mana" resource
      // TODO: Make this more flexible for different resource types
      if (!resource_pool->can_spend(mana_cost)) {
        return false;
      }
    }
  }

  return true;
}

void AbilityComponent::_begin_cast(int slot, Object* target) {
  if (slot < 0 || slot >= 4) {
    return;
  }

  Ref<AbilityDefinition> ability = ability_slots[slot];
  if (ability == nullptr) {
    return;
  }

  casting_slot = slot;
  casting_target = target;
  casting_timer = 0.0f;
  casting_state = static_cast<int>(CastState::CASTING);

  emit_signal("ability_cast_started", slot, target);

  UtilityFunctions::print("[AbilityComponent] Began casting ability slot " +
                          String::num(slot));
}

void AbilityComponent::_execute_ability(int slot) {
  if (slot < 0 || slot >= 4) {
    return;
  }

  Ref<AbilityDefinition> ability = ability_slots[slot];
  if (ability == nullptr) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    return;
  }

  // Spend mana if we have it
  if (resource_pool != nullptr) {
    float mana_cost = ability->get_mana_cost();
    if (mana_cost > 0.0f) {
      resource_pool->try_spend(mana_cost);
    }
  }

  // Get the effect and execute it
  Ref<AbilityEffect> effect = ability->get_effect();
  if (effect != nullptr) {
    // For position-based abilities (POINT_TARGET, AREA), use execute_at_point
    // For unit-targeted abilities, use execute with the target
    int targeting_type = ability->get_targeting_type();
    if (targeting_type == 1 || targeting_type == 2) {  // POINT_TARGET or AREA
      // Use position-based execution centered at click location
      effect->execute_at_point(owner, casting_point, ability.ptr());
    } else {
      // Use target-based execution
      effect->execute(owner, casting_target, ability.ptr());
    }
  }

  // Apply cooldown
  _apply_cooldown(slot);

  emit_signal("ability_executed", slot, casting_target);

  UtilityFunctions::print("[AbilityComponent] Executed ability slot " +
                          String::num(slot));
}

void AbilityComponent::_apply_cooldown(int slot) {
  if (slot < 0 || slot >= 4) {
    return;
  }

  Ref<AbilityDefinition> ability = ability_slots[slot];
  if (ability == nullptr) {
    return;
  }

  float cooldown = ability->get_cooldown();
  cooldown_timers[slot] = cooldown;

  emit_signal("ability_cooldown_started", slot, cooldown);
}

void AbilityComponent::_finish_casting() {
  casting_slot = -1;
  casting_target = nullptr;
  casting_timer = 0.0f;
  casting_state = static_cast<int>(CastState::IDLE);
}
