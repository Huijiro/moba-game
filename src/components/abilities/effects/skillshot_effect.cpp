#include "skillshot_effect.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../core/unit.hpp"
#include "../../combat/skillshot_projectile.hpp"
#include "../ability_definition.hpp"
#include "../../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

SkillshotEffect::SkillshotEffect() = default;

SkillshotEffect::~SkillshotEffect() = default;

void SkillshotEffect::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_projectile_speed", "speed"),
                       &SkillshotEffect::set_projectile_speed);
  ClassDB::bind_method(D_METHOD("get_projectile_speed"),
                       &SkillshotEffect::get_projectile_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "projectile_speed"),
               "set_projectile_speed", "get_projectile_speed");

  ClassDB::bind_method(D_METHOD("set_projectile_max_distance", "distance"),
                       &SkillshotEffect::set_projectile_max_distance);
  ClassDB::bind_method(D_METHOD("get_projectile_max_distance"),
                       &SkillshotEffect::get_projectile_max_distance);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "projectile_max_distance"),
               "set_projectile_max_distance", "get_projectile_max_distance");

  ClassDB::bind_method(D_METHOD("set_explosion_radius", "radius"),
                       &SkillshotEffect::set_explosion_radius);
  ClassDB::bind_method(D_METHOD("get_explosion_radius"),
                       &SkillshotEffect::get_explosion_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "explosion_radius"),
               "set_explosion_radius", "get_explosion_radius");

  ClassDB::bind_method(D_METHOD("set_projectile_hit_radius", "radius"),
                       &SkillshotEffect::set_projectile_hit_radius);
  ClassDB::bind_method(D_METHOD("get_projectile_hit_radius"),
                       &SkillshotEffect::get_projectile_hit_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "projectile_hit_radius"),
               "set_projectile_hit_radius", "get_projectile_hit_radius");
}

void SkillshotEffect::execute(Unit* caster,
                              Object* target,
                              const AbilityDefinition* ability) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (caster == nullptr || ability == nullptr) {
    DBG_INFO("SkillshotEffect", "Invalid caster or ability");
    return;
  }

  // For skillshots executed from a target, calculate direction from caster to
  // target If target is a Unit, use its position
  Vector3 direction = Vector3(0, 0, -1);  // Default forward

  if (target != nullptr) {
    Unit* target_unit = godot::Object::cast_to<Unit>(target);
    if (target_unit != nullptr && target_unit->is_inside_tree()) {
      Vector3 caster_pos = caster->get_global_position();
      Vector3 target_pos = target_unit->get_global_position();
      Vector3 to_target = target_pos - caster_pos;
      float distance = to_target.length();
      if (distance > 0.001f) {
        direction = to_target / distance;
      }
      DBG_INFO("SkillshotEffect", "Skillshot targeting unit at " + godot::String::num(target_pos.x) + ", " +
                              godot::String::num(target_pos.z));
    }
  }

  // Create and spawn projectile
  SkillshotProjectile* projectile = memnew(SkillshotProjectile);
  caster->add_child(projectile);
  projectile->setup(caster, direction, ability->get_base_damage(),
                    projectile_speed, projectile_max_distance, explosion_radius,
                    projectile_hit_radius);

  DBG_INFO("SkillshotEffect", "Spawned skillshot projectile");
}

void SkillshotEffect::execute_at_point(Unit* caster,
                                       const Vector3& point,
                                       const AbilityDefinition* ability) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (caster == nullptr || ability == nullptr) {
    DBG_INFO("SkillshotEffect", "Invalid caster or ability");
    return;
  }

  // Calculate direction from caster to clicked point
  Vector3 caster_pos = caster->get_global_position();
  Vector3 to_point = point - caster_pos;
  float distance = to_point.length();

  Vector3 direction = Vector3(0, 0, -1);  // Default forward
  if (distance > 0.001f) {
    direction = to_point / distance;
  }

  DBG_INFO("SkillshotEffect", "Skillshot toward point (" + godot::String::num(point.x) + ", " +
                          godot::String::num(point.z) + ")");

  // Create and spawn projectile
  SkillshotProjectile* projectile = memnew(SkillshotProjectile);
  caster->add_child(projectile);
  projectile->setup(caster, direction, ability->get_base_damage(),
                    projectile_speed, projectile_max_distance, explosion_radius,
                    projectile_hit_radius);

  DBG_INFO("SkillshotEffect", "Spawned skillshot projectile");
}

void SkillshotEffect::set_projectile_speed(float speed) {
  projectile_speed = std::max(0.0f, speed);
}

float SkillshotEffect::get_projectile_speed() const {
  return projectile_speed;
}

void SkillshotEffect::set_projectile_max_distance(float distance) {
  projectile_max_distance = std::max(0.0f, distance);
}

float SkillshotEffect::get_projectile_max_distance() const {
  return projectile_max_distance;
}

void SkillshotEffect::set_explosion_radius(float radius) {
  explosion_radius = std::max(0.0f, radius);
}

float SkillshotEffect::get_explosion_radius() const {
  return explosion_radius;
}

void SkillshotEffect::set_projectile_hit_radius(float radius) {
  projectile_hit_radius = std::max(0.0f, radius);
}

float SkillshotEffect::get_projectile_hit_radius() const {
  return projectile_hit_radius;
}
