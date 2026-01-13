#include "unit.hpp"

#include <cmath>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/navigation_agent3d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Basis;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::NavigationAgent3D;
using godot::Node;
using godot::PackedStringArray;
using godot::PropertyInfo;
using godot::String;
using godot::Transform3D;
using godot::UtilityFunctions;
using godot::Variant;
using godot::Vector3;

Unit::Unit() = default;

Unit::~Unit() = default;

void Unit::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_navigation_agent"),
                       &Unit::get_navigation_agent);
  ClassDB::bind_method(D_METHOD("set_desired_location", "location"),
                       &Unit::set_desired_location);
  ClassDB::bind_method(D_METHOD("get_desired_location"),
                       &Unit::get_desired_location);
  ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "desired_location"),
               "set_desired_location", "get_desired_location");

  ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Unit::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"), &Unit::get_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
}

void Unit::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }
  _cache_navigation_agent();
  frame_count = 0;
}

void Unit::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Safety check - both this node and navigation agent must be in tree to
  // function
  if (!is_inside_tree()) {
    return;
  }

  if (navigation_agent == nullptr) {
    _cache_navigation_agent();
  }
  if (navigation_agent == nullptr) {
    return;
  }

  // Ensure navigation agent is in tree before using it
  if (!navigation_agent->is_inside_tree()) {
    frame_count = 0;  // Reset counter
    return;
  }

  // Wait several frames after entering tree before using navigation
  if (!is_ready) {
    frame_count++;
    if (frame_count < 3) {
      return;
    }
    is_ready = true;
  }

  Vector3 current_target = navigation_agent->get_target_position();

  // Only update if target position has changed
  if (!current_target.is_equal_approx(desired_location)) {
    navigation_agent->set_target_position(desired_location);
  }

  Vector3 current_position = get_global_position();
  Vector3 next_position = navigation_agent->get_next_path_position();
  Vector3 displacement = next_position - current_position;
  float distance = displacement.length();

  // If we're very close to the next position, we've reached the destination
  if (distance < 0.1f) {
    set_velocity(Vector3(0, get_velocity().y, 0));
    move_and_slide();
    return;
  }

  if (distance > 0.001f) {
    Vector3 direction = displacement / distance;
    Vector3 velocity = direction * speed;
    velocity.y = get_velocity().y;
    set_velocity(velocity);

    // Rotate unit to face movement direction instantly (Y axis only)
    Vector3 horizontal_direction =
        Vector3(direction.x, 0, direction.z).normalized();
    if (horizontal_direction.length() > 0.001f) {
      // Calculate target angle (rotation around Y axis)
      float target_angle =
          std::atan2(-horizontal_direction.x, -horizontal_direction.z);

      // Create rotation matrix for Y axis rotation
      float cos_a = std::cos(target_angle);
      float sin_a = std::sin(target_angle);
      Vector3 new_forward = Vector3(-sin_a, 0, -cos_a);
      Vector3 new_right = Vector3(cos_a, 0, -sin_a);
      Vector3 new_up = Vector3(0, 1, 0);

      Basis new_basis = Basis();
      new_basis.set_column(0, new_right);
      new_basis.set_column(1, new_up);
      new_basis.set_column(2, -new_forward);
      set_transform(Transform3D(new_basis, get_transform().origin));
    }
  } else {
    set_velocity(Vector3(0, get_velocity().y, 0));
  }
  move_and_slide();
}

void Unit::set_desired_location(const Vector3& location) {
  desired_location = location;
  if (navigation_agent != nullptr && is_ready) {
    navigation_agent->set_target_position(desired_location);
  }
}

Vector3 Unit::get_desired_location() const {
  return desired_location;
}

void Unit::set_speed(float new_speed) {
  speed = new_speed;
}

float Unit::get_speed() const {
  return speed;
}

PackedStringArray Unit::_get_configuration_warnings() const {
  if (_find_navigation_agent() == nullptr) {
    PackedStringArray warnings;
    warnings.push_back(String("Unit requires a NavigationAgent3D child node."));
    return warnings;
  }
  return PackedStringArray();
}

NavigationAgent3D* Unit::get_navigation_agent() const {
  return navigation_agent;
}

void Unit::_cache_navigation_agent() {
  navigation_agent = _find_navigation_agent();
}

NavigationAgent3D* Unit::_find_navigation_agent() const {
  const int32_t total_children = get_child_count();
  for (int32_t i = 0; i < total_children; ++i) {
    Node* child = get_child(i);
    if (child == nullptr) {
      continue;
    }
    if (auto agent = Object::cast_to<NavigationAgent3D>(child)) {
      return agent;
    }
  }
  return nullptr;
}
