#include "unit.hpp"

#include "attack_component.hpp"
#include "health_component.hpp"
#include "interactable.hpp"
#include "movement_component.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::MethodInfo;
using godot::Node;
using godot::PropertyInfo;
using godot::StringName;
using godot::UtilityFunctions;
using godot::Variant;
using godot::Vector3;

Unit::Unit() = default;

Unit::~Unit() = default;

void Unit::_bind_methods() {
  ClassDB::bind_method(D_METHOD("issue_move_order", "position"),
                       &Unit::issue_move_order);
  ClassDB::bind_method(D_METHOD("issue_attack_order", "target"),
                       &Unit::issue_attack_order);
  ClassDB::bind_method(D_METHOD("issue_interact_order", "target"),
                       &Unit::issue_interact_order);
  ClassDB::bind_method(D_METHOD("stop_order"), &Unit::stop_order);

  ClassDB::bind_method(D_METHOD("set_desired_location", "location"),
                       &Unit::set_desired_location);
  ClassDB::bind_method(D_METHOD("get_desired_location"),
                       &Unit::get_desired_location);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "desired_location"),
               "set_desired_location", "get_desired_location");

  ClassDB::bind_method(D_METHOD("set_auto_attack_range", "range"),
                       &Unit::set_auto_attack_range);
  ClassDB::bind_method(D_METHOD("get_auto_attack_range"),
                       &Unit::get_auto_attack_range);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "auto_attack_range"),
               "set_auto_attack_range", "get_auto_attack_range");

  ClassDB::bind_method(D_METHOD("set_attack_buffer_range", "buffer"),
                       &Unit::set_attack_buffer_range);
  ClassDB::bind_method(D_METHOD("get_attack_buffer_range"),
                       &Unit::get_attack_buffer_range);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_buffer_range"),
               "set_attack_buffer_range", "get_attack_buffer_range");

  ClassDB::bind_method(D_METHOD("set_faction_id", "faction_id"),
                       &Unit::set_faction_id);
  ClassDB::bind_method(D_METHOD("get_faction_id"), &Unit::get_faction_id);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "faction_id"), "set_faction_id",
               "get_faction_id");

  ADD_SIGNAL(MethodInfo("order_changed",
                        PropertyInfo(Variant::INT, "previous_order"),
                        PropertyInfo(Variant::INT, "new_order"),
                        PropertyInfo(Variant::OBJECT, "target")));
}

void Unit::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  movement_component = Object::cast_to<MovementComponent>(
      get_component_by_class("MovementComponent"));
}

void Unit::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Safety check - unit must be in tree to function
  if (!is_inside_tree()) {
    return;
  }

  // Safety check - dead units should not process
  HealthComponent* health_comp = get_health_component();
  if (health_comp != nullptr && health_comp->is_dead()) {
    return;
  }

  // Handle order-specific logic
  bool should_attempt_attack = false;

  if (current_order == OrderType::ATTACK) {
    if (attack_target == nullptr || !attack_target->is_inside_tree()) {
      stop_order();
    } else {
      // Check if target is dead
      HealthComponent* target_health = attack_target->get_health_component();
      if (target_health != nullptr && target_health->is_dead()) {
        stop_order();
      } else {
        const Vector3 target_pos = attack_target->get_global_position();
        desired_location = target_pos;

        // Check if target is within attack range
        Vector3 current_position = get_global_position();
        Vector3 to_target = target_pos - current_position;
        to_target.y = 0.0f;
        const float distance_to_target = to_target.length();

        // Get attack range from AttackComponent
        AttackComponent* attack_comp = get_attack_component();
        float effective_attack_range = auto_attack_range;
        if (attack_comp != nullptr) {
          effective_attack_range = attack_comp->get_attack_range();
        }

        // Hysteresis: stop when within range, but resume only when far enough
        // away. This prevents jitter from continuous start/stop cycles.
        const float resume_distance =
            effective_attack_range + attack_buffer_range;
        if (distance_to_target <= effective_attack_range) {
          // In attack range - stop movement and attempt attack
          should_attempt_attack = true;
          // Still use MovementComponent to face target while attacking
        } else if (distance_to_target <= resume_distance) {
          // In the buffer zone: keep moving toward target
          // MovementComponent will handle the movement and rotation
        }
      }
    }
  } else if (current_order == OrderType::INTERACT) {
    if (interact_target == nullptr || !interact_target->is_inside_tree()) {
      stop_order();
    } else {
      desired_location = interact_target->get_global_position();
    }
  }

  // Get movement from component (or zero if no component)
  // MovementComponent handles movement AND rotation via
  // _face_horizontal_direction()
  Vector3 movement_velocity = Vector3(0, 0, 0);
  if (movement_component != nullptr && movement_component->is_inside_tree()) {
    movement_velocity = movement_component->process_movement(
        delta, desired_location, current_order);
  } else if (movement_component != nullptr) {
    // Component was queued for deletion, clear our reference
    movement_component = nullptr;
  }

  // If we should attack, zero out movement but keep the rotation from above
  if (should_attempt_attack) {
    movement_velocity = Vector3(0, 0, 0);

    AttackComponent* attack_comp = get_attack_component();
    if (attack_comp != nullptr) {
      attack_comp->try_fire_at(attack_target, delta);
    } else {
      UtilityFunctions::push_error(
          "[Unit] ATTACK order requires AttackComponent");
      stop_order();
    }
  }

  // Apply gravity and move
  Vector3 velocity = movement_velocity;
  velocity.y = get_velocity().y;
  set_velocity(velocity);
  move_and_slide();
}

void Unit::issue_move_order(const Vector3& position) {
  _clear_order_targets();
  _set_order(OrderType::MOVE, nullptr);
  desired_location = position;
}

void Unit::issue_attack_order(Unit* target) {
  _clear_order_targets();
  attack_target = target;
  _set_order(OrderType::ATTACK, target);

  if (attack_target != nullptr && attack_target->is_inside_tree()) {
    desired_location = attack_target->get_global_position();
  }
}

void Unit::issue_interact_order(Interactable* target) {
  _clear_order_targets();
  interact_target = target;
  _set_order(OrderType::INTERACT, target);

  if (interact_target != nullptr && interact_target->is_inside_tree()) {
    desired_location = interact_target->get_global_position();
  }
}

void Unit::stop_order() {
  _clear_order_targets();
  _set_order(OrderType::NONE, nullptr);

  // Stop horizontal movement but keep vertical velocity (gravity).
  set_velocity(Vector3(0, get_velocity().y, 0));
}

void Unit::set_desired_location(const Vector3& location) {
  issue_move_order(location);
}

Vector3 Unit::get_desired_location() const {
  return desired_location;
}

void Unit::set_auto_attack_range(float new_range) {
  auto_attack_range = new_range;
}

float Unit::get_auto_attack_range() const {
  return auto_attack_range;
}

void Unit::set_attack_buffer_range(float new_buffer) {
  attack_buffer_range = new_buffer;
  if (attack_buffer_range < 0.0f) {
    attack_buffer_range = 0.0f;
  }
}

float Unit::get_attack_buffer_range() const {
  return attack_buffer_range;
}

void Unit::set_faction_id(int32_t new_faction_id) {
  faction_id = new_faction_id;
}

int32_t Unit::get_faction_id() const {
  return faction_id;
}

void Unit::_set_order(OrderType new_order, godot::Object* new_target) {
  OrderType previous_order = current_order;
  godot::Object* previous_target = current_order_target;

  current_order = new_order;
  current_order_target = new_target;

  if (previous_order != current_order ||
      previous_target != current_order_target) {
    emit_signal("order_changed", static_cast<int>(previous_order),
                static_cast<int>(current_order), current_order_target);
  }
}

void Unit::_clear_order_targets() {
  attack_target = nullptr;
  interact_target = nullptr;
}

Node* Unit::get_component_by_class(const StringName& class_name) const {
  const int32_t total_children = get_child_count();
  for (int32_t i = 0; i < total_children; ++i) {
    Node* child = get_child(i);
    if (child == nullptr) {
      continue;
    }
    if (child->get_class() == class_name) {
      return child;
    }
  }
  return nullptr;
}

HealthComponent* Unit::get_health_component() const {
  Node* component = get_component_by_class("HealthComponent");
  return Object::cast_to<HealthComponent>(component);
}

AttackComponent* Unit::get_attack_component() const {
  Node* component = get_component_by_class("AttackComponent");
  return Object::cast_to<AttackComponent>(component);
}
