#include "skillshot_projectile.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <vector>

#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../../debug/visual_debugger.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Node;
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

  ClassDB::bind_method(D_METHOD("set_hit_radius", "radius"),
                       &SkillshotProjectile::set_hit_radius);
  ClassDB::bind_method(D_METHOD("get_hit_radius"),
                       &SkillshotProjectile::get_hit_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hit_radius"), "set_hit_radius",
               "get_hit_radius");
}

void SkillshotProjectile::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (caster == nullptr || !caster->is_inside_tree()) {
    queue_free();
    return;
  }

  // Move
  Vector3 current_pos = get_global_position();
  Vector3 velocity = direction * speed;
  set_global_position(current_pos + velocity * static_cast<float>(delta));
  travel_distance += speed * delta;

  Vector3 new_pos = get_global_position();

  // Debug: draw collision radius
  VisualDebugger* debugger = VisualDebugger::get_singleton();
  if (debugger != nullptr && debugger->is_debug_enabled()) {
    debugger->draw_circle_xz(new_pos, hit_radius, godot::Color(1, 1, 0, 1), 16,
                             1.0f);
  }

  // Check max distance
  if (travel_distance >= max_distance) {
    DBG_INFO("SkillshotProjectile", "Reached max distance");
    emit_signal("reached_max_range", new_pos);
    queue_free();
    return;
  }

  // Check collision with units
  Node* start = caster->get_parent();
  if (start == nullptr) {
    return;
  }

  std::vector<Node*> to_process;
  to_process.push_back(start);

  while (!to_process.empty()) {
    Node* current = to_process.back();
    to_process.pop_back();

    if (current == nullptr) {
      continue;
    }

    Unit* unit = Object::cast_to<Unit>(current);
    if (unit != nullptr && unit != caster && unit->is_inside_tree()) {
      float distance = new_pos.distance_to(unit->get_global_position());
      if (distance <= hit_radius) {
        DBG_INFO("SkillshotProjectile", "Hit unit: " + unit->get_name());
        emit_signal("hit", unit, new_pos);
        queue_free();
        return;
      }
    }

    for (int i = 0; i < current->get_child_count(); i++) {
      to_process.push_back(current->get_child(i));
    }
  }
}

void SkillshotProjectile::setup(Unit* caster_unit,
                                const Vector3& travel_direction,
                                float travel_speed,
                                float max_range,
                                float collision_radius) {
  caster = caster_unit;
  speed = travel_speed;
  max_distance = max_range;
  hit_radius = collision_radius;

  float dir_length = travel_direction.length();
  if (dir_length > 0.001f) {
    direction = travel_direction / dir_length;
  } else {
    direction = Vector3(0, 0, -1);
  }

  DBG_INFO("SkillshotProjectile",
           "Setup: speed=" + godot::String::num(speed) +
               ", max_distance=" + godot::String::num(max_distance) +
               ", hit_radius=" + godot::String::num(hit_radius));
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

void SkillshotProjectile::set_hit_radius(float radius) {
  hit_radius = std::max(0.0f, radius);
}
float SkillshotProjectile::get_hit_radius() const {
  return hit_radius;
}
