#include "ability_component.hpp"

#include <algorithm>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../core/unit.hpp"
#include "../../debug/visual_debugger.hpp"
#include "../resources/resource_pool_component.hpp"
#include "ability_node.hpp"

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
  // ========== ABILITY SLOT METHODS ==========
  ClassDB::bind_method(D_METHOD("set_ability_scene", "slot", "scene"),
                       &AbilityComponent::set_ability_scene);
  ClassDB::bind_method(D_METHOD("get_ability", "slot"),
                       &AbilityComponent::get_ability);
  ClassDB::bind_method(D_METHOD("has_ability", "slot"),
                       &AbilityComponent::has_ability);
  ClassDB::bind_method(D_METHOD("get_ability_count"),
                       &AbilityComponent::get_ability_count);
  ClassDB::bind_method(D_METHOD("set_ability_count", "count"),
                       &AbilityComponent::set_ability_count);

  // ========== ARRAY-BASED INTERFACE ==========
  ClassDB::bind_method(D_METHOD("set_ability_scenes", "scenes"),
                       &AbilityComponent::set_ability_scenes);
  ClassDB::bind_method(D_METHOD("get_ability_scenes"),
                       &AbilityComponent::get_ability_scenes);
  ClassDB::bind_method(D_METHOD("set_abilities", "abilities"),
                       &AbilityComponent::set_abilities);
  ClassDB::bind_method(D_METHOD("get_abilities"),
                       &AbilityComponent::get_abilities);

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

  // ========== PROPERTIES ==========
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "ability_scenes",
                            godot::PROPERTY_HINT_ARRAY_TYPE, "PackedScene"),
               "set_ability_scenes", "get_ability_scenes");

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
  ADD_SIGNAL(godot::MethodInfo("ability_channel_tick",
                               PropertyInfo(Variant::INT, "slot"),
                               PropertyInfo(Variant::OBJECT, "target")));
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

  // Initialize cooldown timers - size based on ability_scenes array
  if (cooldown_timers.size() != static_cast<size_t>(ability_scenes.size())) {
    cooldown_timers.resize(ability_scenes.size(), 0.0f);
  }
}

void AbilityComponent::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Update cooldown timers
  for (size_t i = 0; i < cooldown_timers.size(); i++) {
    if (cooldown_timers[i] > 0.0f) {
      cooldown_timers[i] -= delta;
      if (cooldown_timers[i] < 0.0f) {
        cooldown_timers[i] = 0.0f;
      }
    }
  }

  // Handle casting state machine
  if (casting_slot >= 0 &&
      casting_slot < static_cast<int>(ability_scenes.size())) {
    AbilityNode* ability = get_ability(casting_slot);
    if (ability == nullptr) {
      _finish_casting();
      return;
    }

    // Advance casting timer
    casting_timer += delta;

    // Get cast point timing
    float cast_duration = 0.0f;
    int cast_type = ability->get_cast_type();
    int targeting_type = ability->get_targeting_type();

    // Check range for channel abilities with unit targets
    if (cast_type == static_cast<int>(CastType::CHANNEL) &&
        targeting_type == static_cast<int>(TargetingType::UNIT_TARGET) &&
        casting_target != nullptr) {
      Unit* caster = get_unit();
      Unit* target_unit = Object::cast_to<Unit>(casting_target);
      if (target_unit != nullptr && target_unit->is_inside_tree()) {
        Vector3 caster_pos = caster->get_global_position();
        Vector3 target_pos = target_unit->get_global_position();
        float distance = caster_pos.distance_to(target_pos);
        float range = ability->get_range();

        // Debug visualization: Draw line between caster and target
        VisualDebugger* debugger = VisualDebugger::get_singleton();
        if (debugger != nullptr && debugger->is_debug_enabled()) {
          // Draw white line from caster to target with thickness
          debugger->draw_line(caster_pos, target_pos, godot::Color(1, 1, 1, 1),
                              1.0f);
        }

        if (range > 0.0f && distance > range) {
          // Target out of range - interrupt channel
          UtilityFunctions::print(
              "[AbilityComponent] Channel interrupted: target out of range (" +
              String::num(distance, 1) + "m > " + String::num(range, 1) + "m)");
          _finish_casting();
          return;
        }
      } else {
        // Target no longer exists or not in tree - interrupt channel
        UtilityFunctions::print(
            "[AbilityComponent] Channel interrupted: target no longer valid");
        _finish_casting();
        return;
      }
    }

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

        // Initialize tick timer for channel abilities
        if (cast_type == static_cast<int>(CastType::CHANNEL)) {
          float tick_interval = ability->get_channel_tick_interval();
          next_tick_time = (tick_interval > 0.0f) ? tick_interval : 999999.0f;
        }
      }
    } else {
      // Instant cast - execute immediately
      if (casting_state == static_cast<int>(CastState::CASTING)) {
        emit_signal("ability_cast_point_reached", casting_slot, casting_target);
        _execute_ability(casting_slot);
        casting_state = static_cast<int>(CastState::ON_COOLDOWN);
      }
    }

    // Handle channel ticking (periodic damage)
    if (cast_type == static_cast<int>(CastType::CHANNEL) &&
        casting_state == static_cast<int>(CastState::ON_COOLDOWN)) {
      float tick_interval = ability->get_channel_tick_interval();
      if (tick_interval > 0.0f && casting_timer >= next_tick_time) {
        // Fire a tick of damage
        emit_signal("ability_channel_tick", casting_slot, casting_target);
        _execute_ability(casting_slot);
        next_tick_time += tick_interval;
      }
    }

    // Check if casting is finished
    if (casting_timer >= cast_duration && cast_duration > 0.0f) {
      _finish_casting();
    }
  }
}

// ========== ABILITY SLOT MANAGEMENT ==========

void AbilityComponent::set_ability_scene(int slot,
                                         const Ref<PackedScene>& scene) {
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
    UtilityFunctions::print("[AbilityComponent] Invalid slot: " +
                            String::num(slot));
    return;
  }
  ability_scenes[slot] = scene;
  // Ensure cooldown timers array is sized correctly
  if (cooldown_timers.size() != static_cast<size_t>(ability_scenes.size())) {
    cooldown_timers.resize(ability_scenes.size(), 0.0f);
  }
}

AbilityNode* AbilityComponent::get_ability(int slot) {
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
    return nullptr;
  }
  Variant scene_variant = ability_scenes[slot];
  if (scene_variant.get_type() != Variant::OBJECT) {
    return nullptr;
  }
  return Object::cast_to<AbilityNode>(scene_variant);
}

bool AbilityComponent::has_ability(int slot) {
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
    return false;
  }
  return get_ability(slot) != nullptr;
}

int AbilityComponent::get_ability_count() const {
  return static_cast<int>(ability_scenes.size());
}

void AbilityComponent::set_ability_count(int count) {
  count = std::max(0, std::min(count, 6));  // Clamp between 0 and 6
  if (static_cast<int>(ability_scenes.size()) != count) {
    ability_scenes.resize(count);
    cooldown_timers.resize(count, 0.0f);
    UtilityFunctions::print("[AbilityComponent] Resized to " +
                            String::num(count) + " ability slots");
  }
}

void AbilityComponent::set_ability_scenes(const godot::Array& scenes) {
  // Instantiate all PackedScene references into AbilityNode instances
  godot::Array instantiated_abilities;
  for (int i = 0; i < scenes.size(); i++) {
    Variant scene_variant = scenes[i];
    if (scene_variant.get_type() == Variant::OBJECT) {
      Ref<PackedScene> scene_ref = scene_variant;
      if (scene_ref.is_valid()) {
        // Instantiate the scene to get the AbilityNode
        Object* instance = scene_ref->instantiate();
        if (instance != nullptr) {
          AbilityNode* ability = Object::cast_to<AbilityNode>(instance);
          if (ability != nullptr) {
            instantiated_abilities.append(ability);
            UtilityFunctions::print("[AbilityComponent] Instantiated ability " +
                                    String::num(i) + ": " +
                                    ability->get_ability_name());
          } else {
            UtilityFunctions::print(
                "[AbilityComponent] Warning: Scene at slot " + String::num(i) +
                " does not contain AbilityNode");
            // Try to free as Node
            godot::Node* node_instance = Object::cast_to<godot::Node>(instance);
            if (node_instance != nullptr) {
              node_instance->queue_free();
            }
            instantiated_abilities.append(Variant());
          }
        } else {
          instantiated_abilities.append(Variant());
        }
      } else {
        instantiated_abilities.append(Variant());
      }
    } else {
      instantiated_abilities.append(Variant());
    }
  }

  ability_scenes = instantiated_abilities;
  // Resize cooldown timers to match
  cooldown_timers.resize(ability_scenes.size(), 0.0f);
  UtilityFunctions::print("[AbilityComponent] Set " +
                          String::num(scenes.size()) + " ability scenes");
}

godot::Array AbilityComponent::get_ability_scenes() const {
  return ability_scenes;
}

void AbilityComponent::set_abilities(const godot::Array& abilities) {
  set_ability_scenes(abilities);
}

godot::Array AbilityComponent::get_abilities() const {
  return get_ability_scenes();
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

void AbilityComponent::interrupt_casting() {
  if (casting_slot >= 0) {
    UtilityFunctions::print("[AbilityComponent] Casting interrupted on slot " +
                            godot::String::num(casting_slot));
    // Apply cooldown even though we interrupted
    if (casting_state == static_cast<int>(CastState::ON_COOLDOWN) ||
        casting_state == static_cast<int>(CastState::CASTING)) {
      _apply_cooldown(casting_slot);
    }
    _finish_casting();
  }
}

bool AbilityComponent::is_on_cooldown(int slot) const {
  if (slot < 0 || slot >= static_cast<int>(cooldown_timers.size())) {
    return false;
  }
  return cooldown_timers[slot] > 0.0f;
}

float AbilityComponent::get_cooldown_remaining(int slot) const {
  if (slot < 0 || slot >= static_cast<int>(cooldown_timers.size())) {
    return 0.0f;
  }
  return cooldown_timers[slot];
}

float AbilityComponent::get_cooldown_duration(int slot) const {
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
    return 0.0f;
  }
  // Can't call get_ability on const method, access directly
  Variant scene_variant = ability_scenes[slot];
  if (scene_variant.get_type() != Variant::OBJECT) {
    return 0.0f;
  }
  AbilityNode* ability = Object::cast_to<AbilityNode>(scene_variant);
  if (ability == nullptr) {
    return 0.0f;
  }
  return ability->get_cooldown();
}

int AbilityComponent::get_cast_state(int slot) const {
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
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
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
    return false;
  }

  // Check if already casting
  if (is_casting()) {
    return false;
  }

  // Check if ability exists
  if (get_ability(slot) == nullptr) {
    return false;
  }

  // Check if off cooldown
  if (is_on_cooldown(slot)) {
    return false;
  }

  return true;
}

ResourcePoolComponent* AbilityComponent::_get_resource_pool(
    const String& pool_id) {
  Unit* owner = get_unit();
  if (owner == nullptr) {
    return nullptr;
  }

  // Try to find a ResourcePoolComponent with matching pool_id
  for (int i = 0; i < owner->get_child_count(); i++) {
    godot::Node* child = owner->get_child(i);
    ResourcePoolComponent* pool = Object::cast_to<ResourcePoolComponent>(child);
    if (pool != nullptr && pool->get_pool_id() == pool_id) {
      return pool;
    }
  }

  // Fallback: return the default resource pool if found
  return Object::cast_to<ResourcePoolComponent>(
      owner->get_component_by_class("ResourcePoolComponent"));
}

bool AbilityComponent::_can_afford(int slot) {
  AbilityNode* ability = get_ability(slot);
  if (ability == nullptr) {
    return false;
  }

  // Check resource cost
  float resource_cost = ability->get_resource_cost();
  if (resource_cost > 0.0f) {
    String pool_id = ability->get_resource_pool_id();
    ResourcePoolComponent* pool = _get_resource_pool(pool_id);
    if (pool == nullptr) {
      UtilityFunctions::print("[AbilityComponent] Warning: No resource pool '" +
                              pool_id + "' found for ability '" +
                              ability->get_ability_name() + "'");
      return false;
    }

    if (!pool->can_spend(resource_cost)) {
      return false;
    }
  }

  return true;
}

void AbilityComponent::_begin_cast(int slot, Object* target) {
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
    return;
  }

  AbilityNode* ability = get_ability(slot);
  if (ability == nullptr) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    return;
  }

  // Validate the ability can execute on this target
  Unit* target_unit = Object::cast_to<Unit>(target);
  if (!ability->can_execute_on_target(owner, target_unit)) {
    UtilityFunctions::print(
        "[AbilityComponent] Ability validation failed for slot " +
        String::num(slot));
    emit_signal("cast_failed", slot, "invalid_target");
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
  if (slot < 0 || slot >= static_cast<int>(ability_scenes.size())) {
    return;
  }

  AbilityNode* ability = get_ability(slot);
  if (ability == nullptr) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    return;
  }

  // Spend resource if needed
  float resource_cost = ability->get_resource_cost();
  if (resource_cost > 0.0f) {
    String pool_id = ability->get_resource_pool_id();
    ResourcePoolComponent* pool = _get_resource_pool(pool_id);
    if (pool != nullptr) {
      pool->try_spend(resource_cost);
    }
  }

  // Execute the ability
  Unit* target_unit = Object::cast_to<Unit>(casting_target);
  ability->execute(owner, target_unit, casting_point);

  // Apply cooldown
  _apply_cooldown(slot);

  emit_signal("ability_executed", slot, casting_target);

  UtilityFunctions::print("[AbilityComponent] Executed ability slot " +
                          String::num(slot));
}

void AbilityComponent::_apply_cooldown(int slot) {
  if (slot < 0 || slot >= static_cast<int>(cooldown_timers.size())) {
    return;
  }

  AbilityNode* ability = get_ability(slot);
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
