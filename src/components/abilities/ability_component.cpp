#include "ability_component.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../common/unit_signals.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../ui/label_registry.hpp"
#include "ability_context.hpp"
#include "ability_node.hpp"
#include "targeting_info.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Object;
using godot::PackedScene;
using godot::PropertyInfo;
using godot::Ref;
using godot::String;
using godot::UtilityFunctions;
using godot::Variant;

AbilityComponent::AbilityComponent() = default;

AbilityComponent::~AbilityComponent() = default;

void AbilityComponent::_bind_methods() {
  // ========== ABILITY SLOTS ==========
  ClassDB::bind_method(D_METHOD("set_ability_scenes", "scenes"),
                       &AbilityComponent::set_ability_scenes);
  ClassDB::bind_method(D_METHOD("get_ability_scenes"),
                       &AbilityComponent::get_ability_scenes);
  ClassDB::bind_method(D_METHOD("get_ability", "slot"),
                       &AbilityComponent::get_ability);
  ClassDB::bind_method(D_METHOD("has_ability", "slot"),
                       &AbilityComponent::has_ability);
  ClassDB::bind_method(D_METHOD("get_ability_count"),
                       &AbilityComponent::get_ability_count);

  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "ability_scenes",
                            godot::PROPERTY_HINT_ARRAY_TYPE, "PackedScene"),
               "set_ability_scenes", "get_ability_scenes");

  // ========== CASTING ==========
  ClassDB::bind_method(D_METHOD("try_cast", "slot", "target"),
                       &AbilityComponent::try_cast);
  ClassDB::bind_method(D_METHOD("try_cast_point", "slot", "point"),
                       &AbilityComponent::try_cast_point);
  ClassDB::bind_method(D_METHOD("is_casting"),
                       &AbilityComponent::is_casting);
  ClassDB::bind_method(D_METHOD("interrupt_casting"),
                       &AbilityComponent::interrupt_casting);

  // ========== SIGNALS ==========
  ADD_SIGNAL(godot::MethodInfo("cast_failed",
                               PropertyInfo(Variant::INT, "slot"),
                               PropertyInfo(Variant::STRING, "reason")));

  // ========== INTERNAL SIGNAL HANDLERS ==========
  ClassDB::bind_method(D_METHOD("_on_completed", "context"),
                       &AbilityComponent::_on_completed);
  ClassDB::bind_method(D_METHOD("_on_cooldown_started", "context"),
                       &AbilityComponent::_on_cooldown_started);
  ClassDB::bind_method(D_METHOD("_on_cooldown_finished", "context"),
                       &AbilityComponent::_on_cooldown_finished);
  ClassDB::bind_method(D_METHOD("_on_chase_range_reached", "target"),
                       &AbilityComponent::_on_chase_range_reached);
  ClassDB::bind_method(
      D_METHOD("_on_cast_ability", "slot", "target", "position"),
      &AbilityComponent::_on_cast_ability);
}

void AbilityComponent::_ready() {
  UnitComponent::_ready();

  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    DBG_WARN("AbilityComponent", "No Unit owner found");
    return;
  }

  // Register and connect Unit signals
  owner->register_signal(get_chase_range_reached());
  owner->register_signal(cast_ability);
  owner->register_signal(ability_icon_requested);
  owner->register_signal(ability_cooldown_started);
  owner->register_signal(ability_cooldown_tick);

  owner->connect(get_chase_range_reached(),
                 godot::Callable(this, "_on_chase_range_reached"));
  owner->connect(cast_ability,
                 godot::Callable(this, "_on_cast_ability"));

  // Instantiate ability scenes as children
  _instantiate_abilities();
}

// ========== ABILITY SLOT MANAGEMENT ==========

void AbilityComponent::set_ability_scenes(const godot::Array& scenes) {
  ability_scenes = scenes;
}

godot::Array AbilityComponent::get_ability_scenes() const {
  return ability_scenes;
}

AbilityNode* AbilityComponent::get_ability(int slot) {
  if (slot < 0 || slot >= ability_scenes.size()) {
    return nullptr;
  }

  Variant v = ability_scenes[slot];
  if (v.get_type() != Variant::OBJECT) {
    return nullptr;
  }

  return Object::cast_to<AbilityNode>(static_cast<Object*>(v));
}

bool AbilityComponent::has_ability(int slot) {
  return get_ability(slot) != nullptr;
}

int AbilityComponent::get_ability_count() const {
  return ability_scenes.size();
}

// ========== CASTING ==========

bool AbilityComponent::try_cast(int slot, Object* target) {
  Vector3 pos;
  Unit* unit = Object::cast_to<Unit>(target);
  if (unit != nullptr && unit->is_inside_tree()) {
    pos = unit->get_global_position();
  }
  _on_cast_ability(slot, target, pos);
  return casting_slot == slot;  // true if cast started
}

bool AbilityComponent::try_cast_point(int slot, const Vector3& point) {
  _on_cast_ability(slot, nullptr, point);
  return casting_slot == slot;
}

bool AbilityComponent::is_casting() const {
  return casting_slot >= 0;
}

void AbilityComponent::interrupt_casting() {
  if (casting_slot >= 0) {
    AbilityNode* ability = get_ability(casting_slot);
    if (ability != nullptr) {
      DBG_INFO("AbilityComponent",
               "Interrupted: " + ability->get_ability_name());
      // Emit completed so components clean up (cooldown starts, etc.)
      ability->emit_signal("completed", active_context);
    }
    casting_slot = -1;
    active_context = Ref<AbilityContext>();
  }
}

// ========== DEBUG ==========

void AbilityComponent::register_debug_labels(LabelRegistry* registry) {
  if (!registry) {
    return;
  }

  String status = is_casting() ? String("CASTING_") + String::num(casting_slot)
                               : "IDLE";
  registry->register_property("Ability", "status", status);
}

// ========== INTERNAL ==========

void AbilityComponent::_instantiate_abilities() {
  godot::Array instantiated;

  for (int i = 0; i < ability_scenes.size(); i++) {
    Variant v = ability_scenes[i];
    if (v.get_type() != Variant::OBJECT) {
      instantiated.append(Variant());
      continue;
    }

    Ref<PackedScene> scene = v;
    if (!scene.is_valid()) {
      instantiated.append(Variant());
      continue;
    }

    Object* instance = scene->instantiate();
    AbilityNode* ability = Object::cast_to<AbilityNode>(instance);
    if (ability == nullptr) {
      DBG_WARN("AbilityComponent",
               "Scene at slot " + String::num(i) + " is not an AbilityNode");
      if (instance != nullptr) {
        Object::cast_to<godot::Node>(instance)->queue_free();
      }
      instantiated.append(Variant());
      continue;
    }

    // Add to scene tree so subcomponents get _ready() and can process
    add_child(ability);
    _connect_ability_signals(ability, i);
    instantiated.append(ability);

    DBG_INFO("AbilityComponent",
             "Instantiated ability at slot " + String::num(i) + ": " +
                 ability->get_ability_name());
  }

  ability_scenes = instantiated;
}

void AbilityComponent::_connect_ability_signals(AbilityNode* ability, int slot) {
  // Listen to lifecycle signals from this ability
  ability->connect("completed",
                   godot::Callable(this, "_on_completed"));
  ability->connect("cooldown_started",
                   godot::Callable(this, "_on_cooldown_started"));
  ability->connect("cooldown_finished",
                   godot::Callable(this, "_on_cooldown_finished"));
}

void AbilityComponent::_on_completed(const Ref<AbilityContext>& context) {
  if (context.is_null()) {
    return;
  }

  int slot = context->get_slot();
  if (slot == casting_slot) {
    DBG_INFO("AbilityComponent",
             "Cast finished for slot " + String::num(slot));
    casting_slot = -1;
    active_context = Ref<AbilityContext>();

    // Stop movement after cast
    Unit* owner = get_unit();
    if (owner != nullptr) {
      owner->relay(get_stop_requested());
    }
  }
}

void AbilityComponent::_on_cooldown_started(const Ref<AbilityContext>& context) {
  if (context.is_null()) {
    return;
  }

  int slot = context->get_slot();
  Unit* owner = get_unit();
  if (owner != nullptr) {
    // Relay to UI
    owner->relay(ability_cooldown_started, slot, 0.0f);
  }
}

void AbilityComponent::_on_cooldown_finished(const Ref<AbilityContext>& context) {
  if (context.is_null()) {
    return;
  }

  int slot = context->get_slot();
  DBG_INFO("AbilityComponent",
           "Cooldown finished for slot " + String::num(slot));
}

void AbilityComponent::_on_chase_range_reached(Object* target) {
  if (pending_slot < 0) {
    return;
  }

  DBG_INFO("AbilityComponent", "Chase range reached, retrying cast");
  int slot = pending_slot;
  Object* t = pending_target;
  pending_slot = -1;
  pending_target = nullptr;
  try_cast(slot, t);
}

void AbilityComponent::_on_cast_ability(int slot, Object* target,
                                        const Vector3& position) {
  // Create context with all available info
  AbilityNode* ability = get_ability(slot);
  if (ability == nullptr) {
    emit_signal("cast_failed", slot, "no_ability");
    return;
  }

  if (is_casting()) {
    emit_signal("cast_failed", slot, "already_casting");
    return;
  }

  Ref<AbilityContext> ctx;
  ctx.instantiate();
  ctx->set_caster(get_unit());
  ctx->set_target(Object::cast_to<Unit>(target));
  ctx->set_position(position);
  ctx->set_slot(slot);

  if (!ability->can_cast(ctx)) {
    String reason = ctx->get_block_reason();
    DBG_INFO("AbilityComponent",
             "Cast blocked: " + ability->get_ability_name() + " - " + reason);

    // Chase into range for targeted abilities
    if (reason == "out_of_range" && target != nullptr) {
      pending_slot = slot;
      pending_target = target;
      Unit* owner = get_unit();
      if (owner != nullptr) {
        // Query ability for its range so MovementComponent knows when to stop
        Ref<TargetingInfo> info = ability->get_targeting_info();
        float chase_range = info->get_range();
        owner->relay(get_chase_to_range_requested(), target, chase_range);
      }
      DBG_INFO("AbilityComponent",
               "Chasing target for " + ability->get_ability_name());
      return;
    }

    emit_signal("cast_failed", slot, reason);
    return;
  }

  casting_slot = slot;
  active_context = ctx;
  ability->start_cast(ctx);

  DBG_INFO("AbilityComponent",
           "Started cast: " + ability->get_ability_name());
}
