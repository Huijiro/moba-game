#include "skillshot_projectile.hpp"

#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Node;
using godot::Object;
using godot::PropertyInfo;
using godot::Variant;

SkillshotProjectile::SkillshotProjectile() = default;

SkillshotProjectile::~SkillshotProjectile() = default;

void SkillshotProjectile::_bind_methods() {
  // Signals
  ADD_SIGNAL(godot::MethodInfo("hit", PropertyInfo(Variant::OBJECT, "unit"),
                               PropertyInfo(Variant::VECTOR3, "position")));
  ADD_SIGNAL(godot::MethodInfo("reached_max_range",
                               PropertyInfo(Variant::VECTOR3, "position")));

  ClassDB::bind_method(D_METHOD("set_speed", "speed"),
                       &SkillshotProjectile::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"), &SkillshotProjectile::get_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");

  ClassDB::bind_method(D_METHOD("set_max_distance", "distance"),
                       &SkillshotProjectile::set_max_distance);
  ClassDB::bind_method(D_METHOD("get_max_distance"),
                       &SkillshotProjectile::get_max_distance);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_distance"), "set_max_distance",
               "get_max_distance");

  ClassDB::bind_method(D_METHOD("_on_body_entered", "body"),
                       &SkillshotProjectile::_on_body_entered);
}

void SkillshotProjectile::_ready() {
  // Don't process until setup() is called — prevents template from running
  set_physics_process(false);

  // Disable monitoring until setup — template shouldn't detect collisions
  set_monitoring(false);
}

void SkillshotProjectile::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (caster == nullptr || !caster->is_inside_tree()) {
    queue_free();
    return;
  }

  // Move forward
  Vector3 current_pos = get_global_position();
  Vector3 velocity = direction * speed;
  set_global_position(current_pos + velocity * static_cast<float>(delta));
  travel_distance += speed * delta;

  // Check max distance
  if (travel_distance >= max_distance) {
    Vector3 final_pos = get_global_position();
    emit_signal("reached_max_range", final_pos);
    queue_free();
    return;
  }
}

void SkillshotProjectile::_on_body_entered(godot::Node3D* body) {
  Unit* unit = Object::cast_to<Unit>(body);
  if (unit == nullptr || unit == caster) {
    return;
  }

  Vector3 hit_pos = get_global_position();
  DBG_INFO("SkillshotProjectile", "Hit unit: " + unit->get_name());
  emit_signal("hit", unit, hit_pos);
  queue_free();
}

void SkillshotProjectile::setup(Unit* caster_unit,
                                const Vector3& travel_direction,
                                float travel_speed,
                                float max_range) {
  caster = caster_unit;
  speed = travel_speed;
  max_distance = max_range;

  float dir_length = travel_direction.length();
  if (dir_length > 0.001f) {
    direction = travel_direction / dir_length;
  } else {
    direction = Vector3(0, 0, -1);
  }

  // Enable physics and collision detection
  set_physics_process(true);
  set_monitoring(true);

  // Connect to body_entered for collision detection
  connect("body_entered", godot::Callable(this, "_on_body_entered"));
}

Unit* SkillshotProjectile::get_caster() const {
  return caster;
}

void SkillshotProjectile::set_speed(float s) {
  speed = std::max(0.0f, s);
}

float SkillshotProjectile::get_speed() const {
  return speed;
}

void SkillshotProjectile::set_max_distance(float distance) {
  max_distance = std::max(0.0f, distance);
}

float SkillshotProjectile::get_max_distance() const {
  return max_distance;
}
