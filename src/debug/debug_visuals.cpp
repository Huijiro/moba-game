#include "debug_visuals.hpp"

#include <cmath>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::StandardMaterial3D;
using godot::UtilityFunctions;
using godot::Variant;

DebugVisuals* DebugVisuals::singleton_instance = nullptr;

DebugVisuals::DebugVisuals() = default;

DebugVisuals::~DebugVisuals() = default;

void DebugVisuals::_bind_methods() {
  ClassDB::bind_method(D_METHOD("draw_line", "from", "to", "color", "width"),
                       &DebugVisuals::draw_line, DEFVAL(Color(1, 1, 1, 1)),
                       DEFVAL(0.1f));
  ClassDB::bind_method(
      D_METHOD("draw_sphere", "center", "radius", "color", "segments"),
      &DebugVisuals::draw_sphere, DEFVAL(Color(1, 1, 1, 1)), DEFVAL(16));
  ClassDB::bind_method(
      D_METHOD("draw_circle", "center", "radius", "color", "segments"),
      &DebugVisuals::draw_circle, DEFVAL(Color(1, 1, 1, 1)), DEFVAL(32));
  ClassDB::bind_method(D_METHOD("draw_box", "center", "size", "color"),
                       &DebugVisuals::draw_box, DEFVAL(Color(1, 1, 1, 1)));
  ClassDB::bind_method(D_METHOD("draw_cone", "apex", "direction", "height",
                                "base_radius", "color"),
                       &DebugVisuals::draw_cone, DEFVAL(Color(1, 1, 1, 1)));

  ClassDB::bind_method(D_METHOD("draw_aoe_radius", "center", "radius", "color"),
                       &DebugVisuals::draw_aoe_radius,
                       DEFVAL(Color(0, 1, 0, 0.3f)));
  ClassDB::bind_method(D_METHOD("draw_targeting_line", "from", "to", "color"),
                       &DebugVisuals::draw_targeting_line,
                       DEFVAL(Color(1, 1, 0, 1)));
  ClassDB::bind_method(D_METHOD("draw_collision_point", "point", "color"),
                       &DebugVisuals::draw_collision_point,
                       DEFVAL(Color(1, 0, 0, 1)));
  ClassDB::bind_method(D_METHOD("draw_projectile_path", "from", "direction",
                                "distance", "color"),
                       &DebugVisuals::draw_projectile_path,
                       DEFVAL(Color(0, 0, 1, 1)));

  ClassDB::bind_method(D_METHOD("set_debug_enabled", "enabled"),
                       &DebugVisuals::set_debug_enabled);
  ClassDB::bind_method(D_METHOD("is_debug_enabled"),
                       &DebugVisuals::is_debug_enabled);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_enabled"),
               "set_debug_enabled", "is_debug_enabled");

  ClassDB::bind_method(D_METHOD("set_debug_lifetime", "lifetime"),
                       &DebugVisuals::set_debug_lifetime);
  ClassDB::bind_method(D_METHOD("get_debug_lifetime"),
                       &DebugVisuals::get_debug_lifetime);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "debug_lifetime"),
               "set_debug_lifetime", "get_debug_lifetime");
}

void DebugVisuals::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Create mesh instance for debug rendering
  mesh_instance = memnew(MeshInstance3D);
  add_child(mesh_instance);
  mesh_instance->set_name("DebugMeshInstance");

  // Set singleton
  if (singleton_instance == nullptr) {
    singleton_instance = this;
    UtilityFunctions::print("[DebugVisuals] Singleton initialized");
  }
}

void DebugVisuals::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (!debug_enabled) {
    if (mesh_instance != nullptr) {
      mesh_instance->set_visible(false);
    }
    return;
  }

  if (mesh_instance != nullptr) {
    mesh_instance->set_visible(true);
  }
}

void DebugVisuals::draw_line(const Vector3& from,
                             const Vector3& to,
                             const Color& color,
                             float width) {
  if (!debug_enabled || mesh_instance == nullptr) {
    return;
  }

  // Create a simple line using a thin box
  Vector3 direction = to - from;
  float length = direction.length();

  if (length < 0.001f) {
    return;
  }

  Vector3 center = (from + to) * 0.5f;
  Vector3 size(width, width, length);

  // Would be rendered as a box, simplified for now
  UtilityFunctions::print(
      "[DebugVisuals] Line from " + godot::String::num(from.x) + " to " +
      godot::String::num(to.x) + " color: " + godot::String(color));
}

void DebugVisuals::draw_sphere(const Vector3& center,
                               float radius,
                               const Color& color,
                               int segments) {
  if (!debug_enabled) {
    return;
  }

  UtilityFunctions::print(
      "[DebugVisuals] Sphere at " + godot::String::num(center.x) + ", " +
      godot::String::num(center.z) + " radius: " + godot::String::num(radius));
}

void DebugVisuals::draw_circle(const Vector3& center,
                               float radius,
                               const Color& color,
                               int segments) {
  if (!debug_enabled) {
    return;
  }

  UtilityFunctions::print(
      "[DebugVisuals] Circle at " + godot::String::num(center.x) + ", " +
      godot::String::num(center.z) + " radius: " + godot::String::num(radius));
}

void DebugVisuals::draw_box(const Vector3& center,
                            const Vector3& size,
                            const Color& color) {
  if (!debug_enabled) {
    return;
  }

  UtilityFunctions::print("[DebugVisuals] Box at " +
                          godot::String::num(center.x) + ", " +
                          godot::String::num(center.z));
}

void DebugVisuals::draw_cone(const Vector3& apex,
                             const Vector3& direction,
                             float height,
                             float base_radius,
                             const Color& color) {
  if (!debug_enabled) {
    return;
  }

  UtilityFunctions::print("[DebugVisuals] Cone at " +
                          godot::String::num(apex.x) + ", " +
                          godot::String::num(apex.z));
}

void DebugVisuals::draw_aoe_radius(const Vector3& center,
                                   float radius,
                                   const Color& color) {
  if (!debug_enabled) {
    return;
  }

  UtilityFunctions::print(
      "[DebugVisuals:AoE] Circle at (" + godot::String::num(center.x) + ", " +
      godot::String::num(center.z) + ") radius: " + godot::String::num(radius) +
      " color: RGBA(" + godot::String::num(color.r) + "," +
      godot::String::num(color.g) + "," + godot::String::num(color.b) + "," +
      godot::String::num(color.a) + ")");
}

void DebugVisuals::draw_targeting_line(const Vector3& from,
                                       const Vector3& to,
                                       const Color& color) {
  if (!debug_enabled) {
    return;
  }

  Vector3 direction = to - from;
  float distance = direction.length();

  UtilityFunctions::print(
      "[DebugVisuals:TargetingLine] From (" + godot::String::num(from.x) +
      ", " + godot::String::num(from.z) + ") To (" + godot::String::num(to.x) +
      ", " + godot::String::num(to.z) +
      ") distance: " + godot::String::num(distance) + " color: YELLOW");
}

void DebugVisuals::draw_collision_point(const Vector3& point,
                                        const Color& color) {
  if (!debug_enabled) {
    return;
  }

  UtilityFunctions::print("[DebugVisuals:Collision] Point at (" +
                          godot::String::num(point.x) + ", " +
                          godot::String::num(point.z) + ") RED");
}

void DebugVisuals::draw_projectile_path(const Vector3& from,
                                        const Vector3& direction,
                                        float distance,
                                        const Color& color) {
  if (!debug_enabled) {
    return;
  }

  Vector3 end = from + direction * distance;

  UtilityFunctions::print(
      "[DebugVisuals:ProjectilePath] From (" + godot::String::num(from.x) +
      ", " + godot::String::num(from.z) + ") Direction (" +
      godot::String::num(direction.x) + ", " + godot::String::num(direction.z) +
      ") Distance: " + godot::String::num(distance) + " End: (" +
      godot::String::num(end.x) + ", " + godot::String::num(end.z) + ")");
}

void DebugVisuals::set_debug_enabled(bool enabled) {
  debug_enabled = enabled;
  UtilityFunctions::print("[DebugVisuals] Debug " +
                          godot::String(enabled ? "enabled" : "disabled"));
}

bool DebugVisuals::is_debug_enabled() const {
  return debug_enabled;
}

void DebugVisuals::set_debug_lifetime(float lifetime) {
  debug_lifetime = std::max(0.01f, lifetime);
}

float DebugVisuals::get_debug_lifetime() const {
  return debug_lifetime;
}

DebugVisuals* DebugVisuals::get_singleton() {
  return singleton_instance;
}

void DebugVisuals::_render_frame() {
  // Frame rendering logic would go here
  // For now, using console output as fallback
}
