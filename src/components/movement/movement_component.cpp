#include "movement_component.hpp"

#include <cmath>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include "../../common/unit_signals.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_utils.hpp"
#include "../health/health_component.hpp"
#include "../ui/label_registry.hpp"

using godot::Basis;
using godot::Callable;
using godot::CharacterBody3D;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Node;
using godot::Object;
using godot::PropertyInfo;
using godot::StringName;
using godot::Transform3D;
using godot::Variant;
using godot::Vector3;

MovementComponent::MovementComponent() = default;

MovementComponent::~MovementComponent() = default;

void MovementComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_speed", "speed"),
                       &MovementComponent::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"), &MovementComponent::get_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");

  ClassDB::bind_method(D_METHOD("set_rotation_speed", "speed"),
                       &MovementComponent::set_rotation_speed);
  ClassDB::bind_method(D_METHOD("get_rotation_speed"),
                       &MovementComponent::get_rotation_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation_speed"),
               "set_rotation_speed", "get_rotation_speed");

  ClassDB::bind_method(D_METHOD("set_desired_location", "location"),
                       &MovementComponent::set_desired_location);
  ClassDB::bind_method(D_METHOD("get_desired_location"),
                       &MovementComponent::get_desired_location);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "desired_location"),
               "set_desired_location", "get_desired_location");

  ClassDB::bind_method(D_METHOD("is_at_destination"),
                       &MovementComponent::is_at_destination);

  // Bind signal callback methods
  ClassDB::bind_method(D_METHOD("_on_owner_unit_died", "source"),
                       &MovementComponent::_on_owner_unit_died);
  ClassDB::bind_method(D_METHOD("_on_move_requested", "position"),
                       &MovementComponent::_on_move_requested);
  ClassDB::bind_method(D_METHOD("_on_attack_requested", "target", "position"),
                       &MovementComponent::_on_attack_requested);
  ClassDB::bind_method(D_METHOD("_on_chase_requested", "target", "position"),
                       &MovementComponent::_on_chase_requested);
  ClassDB::bind_method(D_METHOD("_on_chase_to_range_requested", "target",
                                "position", "desired_range"),
                       &MovementComponent::_on_chase_to_range_requested);
  ClassDB::bind_method(D_METHOD("_on_stop_requested"),
                       &MovementComponent::_on_stop_requested);
  ClassDB::bind_method(D_METHOD("_on_interact_requested", "target", "position"),
                       &MovementComponent::_on_interact_requested);
}

void MovementComponent::_ready() {
  frame_count = 0;
  is_ready = false;

  Unit* owner = get_owner_unit();
  if (owner != nullptr) {
    // Register signals that this component uses
    owner->register_signal(move_requested);
    owner->register_signal(attack_requested);
    owner->register_signal(chase_requested);
    owner->register_signal(chase_to_range_requested);
    owner->register_signal(chase_range_reached);
    owner->register_signal(stop_requested);
    owner->register_signal(interact_requested);

    // Connect to health component death signal if it exists
    // Iterate through siblings to find HealthComponent
    for (int i = 0; i < owner->get_child_count(); ++i) {
      Node* child = owner->get_child(i);
      if (child == nullptr)
        continue;
      HealthComponent* health_comp = Object::cast_to<HealthComponent>(child);
      if (health_comp != nullptr) {
        health_comp->connect(StringName("died"),
                             Callable(this, StringName("_on_owner_unit_died")));
        break;
      }
    }

    // Connect to Unit's movement-related signals
    owner->connect(move_requested,
                   Callable(this, StringName("_on_move_requested")));
    owner->connect(attack_requested,
                   Callable(this, StringName("_on_attack_requested")));
    owner->connect(chase_requested,
                   Callable(this, StringName("_on_chase_requested")));
    owner->connect(chase_to_range_requested,
                   Callable(this, StringName("_on_chase_to_range_requested")));
    owner->connect(stop_requested,
                   Callable(this, StringName("_on_stop_requested")));
    owner->connect(interact_requested,
                   Callable(this, StringName("_on_interact_requested")));
  }
}

void MovementComponent::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Get the parent CharacterBody3D (Unit)
  CharacterBody3D* body = Object::cast_to<CharacterBody3D>(get_parent());
  if (body == nullptr) {
    return;
  }

  // Update desired location if actively chasing a target
  if (chase_target != nullptr && chase_target->is_inside_tree()) {
    set_desired_location(chase_target->get_global_position());

    // Check if we've reached desired range for chase
    float distance_to_target = body->get_global_position().distance_to(
        chase_target->get_global_position());
    bool now_in_range = distance_to_target <= chase_desired_range;

    if (now_in_range && !was_chase_in_range) {
      // Just reached range - emit signal
      Unit* owner = get_owner_unit();
      if (owner != nullptr) {
        owner->relay(get_chase_range_reached(), chase_target);
      }
      was_chase_in_range = true;
    } else if (!now_in_range) {
      // Out of range again
      was_chase_in_range = false;
    }
  }

  // Get movement velocity from our logic
  Vector3 movement_velocity = process_movement(delta, desired_location);

  // Apply gravity and move
  Vector3 velocity = movement_velocity;
  velocity.y = body->get_velocity().y;  // Preserve vertical velocity (gravity)
  body->set_velocity(velocity);
  body->move_and_slide();
}

void MovementComponent::set_speed(float new_speed) {
  speed = new_speed;
}

float MovementComponent::get_speed() const {
  return speed;
}

void MovementComponent::set_rotation_speed(float new_rotation_speed) {
  rotation_speed = new_rotation_speed;
}

float MovementComponent::get_rotation_speed() const {
  return rotation_speed;
}

Vector3 MovementComponent::process_movement(double delta,
                                            const Vector3& target_location) {
  // Safety checks
  Unit* owner = get_owner_unit();
  if (owner == nullptr || !owner->is_inside_tree()) {
    return Vector3(0, 0, 0);
  }

  // If owner unit is dead, don't move
  // Death is handled via the "died" signal - when received, component queues
  // itself for deletion

  // Ensure this component (which IS the NavigationAgent3D) is in tree before
  // using it
  // Also check if we've been queued for deletion
  if (!is_inside_tree()) {
    frame_count = 0;
    return Vector3(0, 0, 0);
  }

  // Don't process if we're queued for deletion
  if (is_queued_for_deletion()) {
    return Vector3(0, 0, 0);
  }

  // Wait several frames after entering tree before using navigation
  if (!is_ready) {
    frame_count++;
    if (frame_count < 3) {
      return Vector3(0, 0, 0);
    }
    is_ready = true;
    set_target_desired_distance(current_target_distance);
  }

  // Update target distance based on stored value (set by signal handlers)
  set_target_desired_distance(current_target_distance);

  // Update navigation target position
  Vector3 current_target = get_target_position();
  if (!current_target.is_equal_approx(target_location)) {
    set_target_position(target_location);
  }

  // Get next path position and calculate velocity
  Vector3 current_position = owner->get_global_position();
  Vector3 next_position = get_next_path_position();
  Vector3 displacement = next_position - current_position;
  float distance = displacement.length();

  Vector3 velocity = Vector3(0, 0, 0);

  // Calculate direction to target for rotation
  Vector3 direction = Vector3(0, 0, 0);
  if (distance > 0.001f) {
    direction = displacement / distance;
    velocity = direction * speed;
  } else if (distance >= 0.0f) {
    // Calculate direction to the actual target (for rotation when near
    // destination)
    Vector3 to_target = target_location - current_position;
    to_target.y = 0.0f;
    float target_distance = to_target.length();
    if (target_distance > 0.001f) {
      direction = to_target / target_distance;
    }
  }

  // Always rotate to face the direction (whether moving or stopped)
  if (distance >= 0.001f || direction.length() > 0.001f) {
    _face_horizontal_direction(direction);
  }

  return velocity;
}

void MovementComponent::_face_horizontal_direction(const Vector3& direction) {
  Unit* owner = get_owner_unit();
  if (owner == nullptr || !owner->is_inside_tree()) {
    return;
  }

  Vector3 horizontal_direction =
      Vector3(direction.x, 0, direction.z).normalized();
  if (horizontal_direction.length() <= 0.001f) {
    return;
  }

  float target_angle =
      std::atan2(-horizontal_direction.x, -horizontal_direction.z);

  float cos_a = std::cos(target_angle);
  float sin_a = std::sin(target_angle);
  Vector3 new_forward = Vector3(-sin_a, 0, -cos_a);
  Vector3 new_right = Vector3(cos_a, 0, -sin_a);
  Vector3 new_up = Vector3(0, 1, 0);

  Basis new_basis = Basis();
  new_basis.set_column(0, new_right);
  new_basis.set_column(1, new_up);
  new_basis.set_column(2, -new_forward);
  owner->set_transform(Transform3D(new_basis, owner->get_transform().origin));
}

void MovementComponent::set_desired_location(const Vector3& location) {
  desired_location = location;
}

Vector3 MovementComponent::get_desired_location() const {
  return desired_location;
}

bool MovementComponent::is_at_destination() const {
  // Cast away const since is_navigation_finished() isn't const but we just
  // query state
  return const_cast<MovementComponent*>(this)->is_navigation_finished();
}

Unit* MovementComponent::get_owner_unit() const {
  // Check if we're still in the tree - if not, parent might be invalid
  if (!is_inside_tree()) {
    return nullptr;
  }

  Node* parent = get_parent();
  if (parent == nullptr) {
    return nullptr;
  }
  return Object::cast_to<Unit>(parent);
}

void MovementComponent::_on_owner_unit_died(godot::Object* source) {
  // When the owner unit dies, queue this movement component for deletion
  // Dead units should not have a movement component
  if (is_inside_tree()) {
    queue_free();
  }
}

void MovementComponent::_on_move_requested(const Vector3& position) {
  // Static movement - no chase target
  chase_target = nullptr;
  set_desired_location(position);
  current_target_distance = 0.0f;
}

void MovementComponent::_on_attack_requested(godot::Object* target,
                                             const Vector3& position) {
  // Attack movement - chase target within attack range
  // Store target and maintain attack range distance
  chase_target = Object::cast_to<Unit>(target);
  if (chase_target != nullptr && chase_target->is_inside_tree()) {
    set_desired_location(chase_target->get_global_position());
  } else {
    // Fallback to position if target invalid
    set_desired_location(position);
  }
  current_target_distance = 2.5f;
}

void MovementComponent::_on_chase_requested(godot::Object* target,
                                            const Vector3& position) {
  // Chase orders - follow target with no distance constraint
  chase_target = Object::cast_to<Unit>(target);
  if (chase_target != nullptr && chase_target->is_inside_tree()) {
    set_desired_location(chase_target->get_global_position());
  } else {
    // Fallback to position if target invalid
    set_desired_location(position);
  }
  current_target_distance = 0.0f;
}

void MovementComponent::_on_chase_to_range_requested(godot::Object* target,
                                                     const Vector3& position,
                                                     float desired_range) {
  // Chase orders with desired range - follow target until in range
  chase_target = Object::cast_to<Unit>(target);
  chase_desired_range = desired_range;
  was_chase_in_range = false;  // Reset range tracking
  if (chase_target != nullptr && chase_target->is_inside_tree()) {
    set_desired_location(chase_target->get_global_position());
  } else {
    // Fallback to position if target invalid
    set_desired_location(position);
  }
  current_target_distance = 0.0f;
}

void MovementComponent::_on_stop_requested() {
  // Stop order - clear all movement targets and stop
  chase_target = nullptr;
  set_desired_location(
      get_owner_unit()->get_global_position());  // Stop at current location
  current_target_distance = 0.0f;
  was_chase_in_range = false;
}

void MovementComponent::_on_interact_requested(godot::Object* target,
                                               const Vector3& position) {
  // Interact movement - move to target position
  chase_target = nullptr;
  set_desired_location(position);
  current_target_distance = 0.0f;
}

void MovementComponent::register_debug_labels(LabelRegistry* registry) {
  if (!registry) {
    return;
  }

  registry->register_property("Movement", "speed", godot::String::num(speed));
  registry->register_property(
      "Movement", "dest",
      DebugUtils::vector3_to_compact_string(desired_location));
  registry->register_property("Movement", "at_dest",
                              is_at_destination() ? "true" : "false");
}
