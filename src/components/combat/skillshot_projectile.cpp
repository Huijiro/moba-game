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

#include "../health/health_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Node;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

SkillshotProjectile::SkillshotProjectile() = default;

SkillshotProjectile::~SkillshotProjectile() = default;

void SkillshotProjectile::_bind_methods() {
  // Signals
  ADD_SIGNAL(godot::MethodInfo(
      "detonated", godot::PropertyInfo(Variant::VECTOR3, "position")));

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

  ClassDB::bind_method(D_METHOD("set_aoe_radius", "radius"),
                       &SkillshotProjectile::set_aoe_radius);
  ClassDB::bind_method(D_METHOD("get_aoe_radius"),
                       &SkillshotProjectile::get_aoe_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "aoe_radius"), "set_aoe_radius",
               "get_aoe_radius");

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

  // Move projectile in its direction
  Vector3 current_pos = get_global_position();
  Vector3 velocity = direction * speed;
  set_global_position(current_pos + velocity * static_cast<float>(delta));
  travel_distance += speed * delta;

  Vector3 new_pos = get_global_position();

  // Debug visualization: Draw projectile collision radius
  VisualDebugger* debugger = VisualDebugger::get_singleton();
  if (debugger != nullptr && debugger->is_debug_enabled()) {
    // Draw collision radius at projectile position (yellow circle with
    // thickness)
    debugger->draw_circle_xz(new_pos, hit_radius, godot::Color(1, 1, 0, 1), 16,
                             1.0f);
  }

  // Check if we've exceeded max distance
  if (travel_distance >= max_distance) {
    DBG_INFO("SkillshotProjectile",
             "Reached max distance " + godot::String::num(max_distance));
    _detonate();
    return;
  }

  // Check for collision with units
  // Simple sphere-cast style detection
  Node* start = caster->get_parent();
  if (start == nullptr) {
    return;
  }

  std::vector<Node*> to_process;
  to_process.push_back(start);

  bool hit_unit = false;
  Unit* hit_target = nullptr;
  while (!to_process.empty() && !hit_unit) {
    Node* current = to_process.back();
    to_process.pop_back();

    if (current == nullptr) {
      continue;
    }

    Unit* unit = Object::cast_to<Unit>(current);
    if (unit != nullptr && unit != caster && unit->is_inside_tree()) {
      float distance = current_pos.distance_to(unit->get_global_position());
      if (distance <= hit_radius) {
        DBG_INFO("SkillshotProjectile", "Hit unit: " + unit->get_name());
        hit_unit = true;
        hit_target = unit;
        _detonate(hit_target);
        break;
      }
    }

    // Add children to process queue
    for (int i = 0; i < current->get_child_count(); i++) {
      to_process.push_back(current->get_child(i));
    }
  }
}

void SkillshotProjectile::_detonate(Unit* hit_target) {
  if (caster == nullptr || !caster->is_inside_tree()) {
    queue_free();
    return;
  }

  Vector3 explosion_center = get_global_position();

  // Call detonation callback if set (indicates explosion effect)
  bool has_explosion = (on_detonated != nullptr);
  if (on_detonated != nullptr && caster != nullptr) {
    on_detonated(caster, explosion_center);
  }

  // If we have a specific hit target (from collision), damage only that unit
  if (hit_target != nullptr && hit_target->is_inside_tree()) {
    DBG_INFO("SkillshotProjectile",
             "Detonating at (" + godot::String::num(explosion_center.x) + ", " +
                 godot::String::num(explosion_center.z) + ")");

    hit_target->relay("take_damage", damage, caster);
    DBG_INFO("SkillshotProjectile", "Hit " + hit_target->get_name() + " for " +
                                        godot::String::num(damage) + " damage");

    DBG_INFO("SkillshotProjectile", "Total hits: 1");
  } else {
    // No specific target - search for units in AoE radius
    _find_and_damage_units();
  }

  // Draw AoE visualization only if there's an explosion effect
  if (has_explosion) {
    VisualDebugger* debugger = VisualDebugger::get_singleton();
    if (debugger != nullptr && debugger->is_debug_enabled()) {
      // Draw AoE explosion radius at detonation point (orange for visibility)
      debugger->draw_circle_xz(explosion_center, aoe_radius,
                               godot::Color(1, 0.5f, 0, 1), 32, 1.0f, false);
    }
  }

  queue_free();
}

void SkillshotProjectile::_find_and_damage_units() {
  if (caster == nullptr || !caster->is_inside_tree()) {
    return;
  }

  Node* start = caster->get_parent();
  if (start == nullptr) {
    return;
  }

  Vector3 explosion_center = get_global_position();
  std::vector<Unit*> affected_units;
  std::vector<Node*> to_process;
  to_process.push_back(start);

  // Find all units in explosion radius
  while (!to_process.empty()) {
    Node* current = to_process.back();
    to_process.pop_back();

    if (current == nullptr) {
      continue;
    }

    Unit* unit = Object::cast_to<Unit>(current);
    if (unit != nullptr && unit != caster && unit->is_inside_tree()) {
      float distance =
          explosion_center.distance_to(unit->get_global_position());
      if (distance <= aoe_radius) {
        affected_units.push_back(unit);
      }
    }

    // Add children to process queue
    for (int i = 0; i < current->get_child_count(); i++) {
      to_process.push_back(current->get_child(i));
    }
  }

  // Apply damage to all units
  int hit_count = 0;
  for (Unit* unit : affected_units) {
    unit->relay("take_damage", damage, caster);
    hit_count++;
    DBG_INFO("SkillshotProjectile", "Hit " + unit->get_name() + " for " +
                                        godot::String::num(damage) + " damage");
  }

  DBG_INFO("SkillshotProjectile",
           "Total hits: " + godot::String::num(hit_count));
}

void SkillshotProjectile::setup(Unit* caster_unit,
                                const Vector3& travel_direction,
                                float damage_amount,
                                float travel_speed,
                                float max_range,
                                float explosion_radius,
                                float collision_radius) {
  caster = caster_unit;
  damage = damage_amount;
  speed = travel_speed;
  max_distance = max_range;
  aoe_radius = explosion_radius;
  hit_radius = collision_radius;

  // Normalize direction
  float dir_length = travel_direction.length();
  if (dir_length > 0.001f) {
    direction = travel_direction / dir_length;
  } else {
    direction = Vector3(0, 0, -1);  // Default forward
  }

  // Start position at caster
  if (caster_unit != nullptr) {
    set_global_position(caster_unit->get_global_position());
  }

  DBG_INFO("SkillshotProjectile",
           "Setup: damage=" + godot::String::num(damage) +
               ", speed=" + godot::String::num(speed) +
               ", max_distance=" + godot::String::num(max_distance) +
               ", aoe_radius=" + godot::String::num(aoe_radius));
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

void SkillshotProjectile::set_aoe_radius(float radius) {
  aoe_radius = std::max(0.0f, radius);
}

float SkillshotProjectile::get_aoe_radius() const {
  return aoe_radius;
}

void SkillshotProjectile::set_hit_radius(float radius) {
  hit_radius = std::max(0.0f, radius);
}

float SkillshotProjectile::get_hit_radius() const {
  return hit_radius;
}
