#include "projectile_visual.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

ProjectileVisual::ProjectileVisual() = default;

ProjectileVisual::~ProjectileVisual() = default;

void ProjectileVisual::_bind_methods() {
  ClassDB::bind_method(D_METHOD("on_spawn", "position", "direction"),
                       &ProjectileVisual::on_spawn);
  ClassDB::bind_method(D_METHOD("on_travel", "position", "distance_traveled"),
                       &ProjectileVisual::on_travel);
  ClassDB::bind_method(D_METHOD("on_impact", "position"),
                       &ProjectileVisual::on_impact);

  ClassDB::bind_method(D_METHOD("set_projectile_speed", "speed"),
                       &ProjectileVisual::set_projectile_speed);
  ClassDB::bind_method(D_METHOD("get_projectile_speed"),
                       &ProjectileVisual::get_projectile_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "projectile_speed"),
               "set_projectile_speed", "get_projectile_speed");

  ClassDB::bind_method(D_METHOD("set_aoe_radius", "radius"),
                       &ProjectileVisual::set_aoe_radius);
  ClassDB::bind_method(D_METHOD("get_aoe_radius"),
                       &ProjectileVisual::get_aoe_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "aoe_radius"), "set_aoe_radius",
               "get_aoe_radius");

  ClassDB::bind_method(D_METHOD("set_impact_radius", "radius"),
                       &ProjectileVisual::set_impact_radius);
  ClassDB::bind_method(D_METHOD("get_impact_radius"),
                       &ProjectileVisual::get_impact_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "impact_radius"),
               "set_impact_radius", "get_impact_radius");
}

void ProjectileVisual::_physics_process(double delta) {
  // Subclasses can override to animate projectile in flight
}

void ProjectileVisual::_on_spawn(const Vector3& position,
                                 const Vector3& direction) {
  UtilityFunctions::print("[ProjectileVisual] Spawned at (" +
                          godot::String::num(position.x) + ", " +
                          godot::String::num(position.z) + ") direction (" +
                          godot::String::num(direction.x) + ", " +
                          godot::String::num(direction.z) + ")");
}

void ProjectileVisual::_on_travel(const Vector3& position,
                                  float distance_traveled) {
  // Called each frame for projectile animation
  // Subclasses can use this for trail effects, rotation, etc.
}

void ProjectileVisual::_on_impact(const Vector3& position) {
  UtilityFunctions::print("[ProjectileVisual] Impact at (" +
                          godot::String::num(position.x) + ", " +
                          godot::String::num(position.z) +
                          ") AoE radius: " + godot::String::num(aoe_radius));
}

void ProjectileVisual::on_spawn(const Vector3& position,
                                const Vector3& direction) {
  set_global_position(position);
  _on_spawn(position, direction);
}

void ProjectileVisual::on_travel(const Vector3& position,
                                 float distance_traveled) {
  set_global_position(position);
  _on_travel(position, distance_traveled);
}

void ProjectileVisual::on_impact(const Vector3& position) {
  set_global_position(position);
  _on_impact(position);
}

void ProjectileVisual::set_projectile_speed(float speed) {
  projectile_speed = speed >= 0.0f ? speed : 30.0f;
}

float ProjectileVisual::get_projectile_speed() const {
  return projectile_speed;
}

void ProjectileVisual::set_aoe_radius(float radius) {
  aoe_radius = radius >= 0.0f ? radius : 5.0f;
}

float ProjectileVisual::get_aoe_radius() const {
  return aoe_radius;
}

void ProjectileVisual::set_impact_radius(float radius) {
  impact_radius = radius >= 0.0f ? radius : 0.5f;
}

float ProjectileVisual::get_impact_radius() const {
  return impact_radius;
}
