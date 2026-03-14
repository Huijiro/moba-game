#include "ability_targeting_handler.hpp"

#include <godot_cpp/core/class_db.hpp>

#include "../common/unit_signals.hpp"
#include "../components/abilities/ability_component.hpp"
#include "../components/abilities/ability_components/cooldown_component.hpp"
#include "../components/abilities/ability_node.hpp"
#include "../components/abilities/targeting_info.hpp"
#include "../core/unit.hpp"
#include "../debug/debug_macros.hpp"
#include "cursor_world_query.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::Ref;
using godot::String;
using godot::Vector3;

AbilityTargetingHandler::AbilityTargetingHandler() = default;
AbilityTargetingHandler::~AbilityTargetingHandler() = default;

void AbilityTargetingHandler::_bind_methods() {
  ClassDB::bind_method(D_METHOD("is_active"),
                       &AbilityTargetingHandler::is_active);
  ClassDB::bind_method(D_METHOD("get_active_slot"),
                       &AbilityTargetingHandler::get_active_slot);
  ClassDB::bind_method(D_METHOD("cancel"),
                       &AbilityTargetingHandler::cancel);
}

void AbilityTargetingHandler::start(int ability_slot, Unit* unit) {
  DBG_INFO("AbilityTargeting", "start() called for slot " +
               godot::String::num(ability_slot));
  caster = unit;
  if (caster == nullptr) {
    DBG_WARN("AbilityTargeting", "No caster unit");
    return;
  }

  AbilityComponent* ability_comp = _find_ability_component(caster);
  if (ability_comp == nullptr) return;

  AbilityNode* ability = ability_comp->get_ability(ability_slot);
  if (ability == nullptr) {
    DBG_INFO("AbilityTargeting",
             "No ability at slot " + String::num(ability_slot));
    return;
  }

  // Check cooldown — don't enter targeting if ability is on cooldown
  for (int i = 0; i < ability->get_child_count(); i++) {
    auto* cd = Object::cast_to<CooldownComponent>(ability->get_child(i));
    if (cd != nullptr && cd->is_on_cooldown()) {
      DBG_INFO("AbilityTargeting", "Ability on cooldown — cannot target");
      return;
    }
  }

  Ref<TargetingInfo> info = ability->get_targeting_info();
  int type = info->get_type();

  if (type == TargetingInfo::NONE) {
    DBG_INFO("AbilityTargeting", "Passive — not castable");
    return;
  }

  if (type == TargetingInfo::SELF) {
    // Self-cast immediately
    Vector3 pos = caster->get_global_position();
    caster->relay(cast_ability, ability_slot, (godot::Object*)caster, pos);
    return;
  }

  active_slot = ability_slot;
  awaiting_unit_target = (type == TargetingInfo::UNIT);

  DBG_INFO("AbilityTargeting",
           "Slot " + String::num(ability_slot) + " waiting for " +
               (awaiting_unit_target ? "unit" : "point") + " target");
}

void AbilityTargetingHandler::handle_click(CursorWorldQuery* cursor) {
  if (active_slot < 0 || caster == nullptr || cursor == nullptr) return;

  if (!cursor->has_hit()) return;

  // Unit-target ability but no unit clicked — stay in targeting
  if (awaiting_unit_target && cursor->get_hit_unit() == nullptr) {
    DBG_INFO("AbilityTargeting", "No valid unit target — click on a unit");
    return;
  }

  caster->relay(cast_ability, active_slot, cursor->get_hit_object(),
                cursor->get_hit_position());

  // Reset
  active_slot = -1;
  awaiting_unit_target = false;
}

void AbilityTargetingHandler::cancel() {
  if (active_slot >= 0) {
    DBG_INFO("AbilityTargeting",
             "Cancelled targeting for slot " + String::num(active_slot));
  }
  active_slot = -1;
  awaiting_unit_target = false;
  caster = nullptr;
}

bool AbilityTargetingHandler::is_active() const {
  return active_slot >= 0;
}

int AbilityTargetingHandler::get_active_slot() const {
  return active_slot;
}

AbilityComponent* AbilityTargetingHandler::_find_ability_component(
    Unit* unit) const {
  if (unit == nullptr) return nullptr;
  for (int i = 0; i < unit->get_child_count(); i++) {
    auto* comp = Object::cast_to<AbilityComponent>(unit->get_child(i));
    if (comp != nullptr) return comp;
  }
  return nullptr;
}
