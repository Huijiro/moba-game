#include "skillshot_debug_renderer.hpp"

#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::BoxMesh;
using godot::ClassDB;
using godot::CylinderMesh;
using godot::D_METHOD;
using godot::Engine;
using godot::MeshInstance3D;
using godot::PropertyInfo;
using godot::Ref;
using godot::SphereMesh;
using godot::StandardMaterial3D;
using godot::UtilityFunctions;
using godot::Variant;

SkillshotDebugRenderer* SkillshotDebugRenderer::singleton_instance = nullptr;

SkillshotDebugRenderer::SkillshotDebugRenderer() = default;

SkillshotDebugRenderer::~SkillshotDebugRenderer() = default;

void SkillshotDebugRenderer::_bind_methods() {
  ClassDB::bind_method(
      D_METHOD("show_targeting_line", "from", "to", "lifetime"),
      &SkillshotDebugRenderer::show_targeting_line, DEFVAL(0.5f));
  ClassDB::bind_method(
      D_METHOD("show_projectile_indicator", "position", "radius", "lifetime"),
      &SkillshotDebugRenderer::show_projectile_indicator, DEFVAL(0.5f),
      DEFVAL(0.05f));
  ClassDB::bind_method(
      D_METHOD("show_projectile_trail", "from", "to", "width", "lifetime"),
      &SkillshotDebugRenderer::show_projectile_trail, DEFVAL(0.2f),
      DEFVAL(0.1f));
  ClassDB::bind_method(
      D_METHOD("show_aoe_explosion", "center", "radius", "lifetime"),
      &SkillshotDebugRenderer::show_aoe_explosion, DEFVAL(0.5f));
  ClassDB::bind_method(D_METHOD("show_unit_hit_indicator", "point", "lifetime"),
                       &SkillshotDebugRenderer::show_unit_hit_indicator,
                       DEFVAL(0.3f));

  ClassDB::bind_method(D_METHOD("set_debug_visuals_enabled", "enabled"),
                       &SkillshotDebugRenderer::set_debug_visuals_enabled);
  ClassDB::bind_method(D_METHOD("is_debug_visuals_enabled"),
                       &SkillshotDebugRenderer::is_debug_visuals_enabled);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_visuals_enabled"),
               "set_debug_visuals_enabled", "is_debug_visuals_enabled");
}

void SkillshotDebugRenderer::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (singleton_instance == nullptr) {
    singleton_instance = this;
    UtilityFunctions::print(
        "[SkillshotDebugRenderer] Initialized as singleton");
  }

  set_name("SkillshotDebugRenderer");
}

void SkillshotDebugRenderer::show_targeting_line(const Vector3& from,
                                                 const Vector3& to,
                                                 float lifetime) {
  if (!debug_visuals_enabled) {
    return;
  }

  Vector3 direction = to - from;
  float distance = direction.length();

  UtilityFunctions::print(
      "[SkillshotDebugRenderer] Targeting line from (" +
      godot::String::num(from.x) + ", " + godot::String::num(from.z) +
      ") to (" + godot::String::num(to.x) + ", " + godot::String::num(to.z) +
      ") distance: " + godot::String::num(distance) +
      " lifetime: " + godot::String::num(lifetime) + "s [YELLOW LINE]");
}

void SkillshotDebugRenderer::show_projectile_indicator(const Vector3& position,
                                                       float radius,
                                                       float lifetime) {
  if (!debug_visuals_enabled) {
    return;
  }

  UtilityFunctions::print(
      "[SkillshotDebugRenderer] Projectile at (" +
      godot::String::num(position.x) + ", " + godot::String::num(position.z) +
      ") radius: " + godot::String::num(radius) + " [CYAN SPHERE]");
}

void SkillshotDebugRenderer::show_projectile_trail(const Vector3& from,
                                                   const Vector3& to,
                                                   float width,
                                                   float lifetime) {
  if (!debug_visuals_enabled) {
    return;
  }

  Vector3 direction = to - from;
  float distance = direction.length();

  UtilityFunctions::print(
      "[SkillshotDebugRenderer] Projectile trail from (" +
      godot::String::num(from.x) + ", " + godot::String::num(from.z) +
      ") to (" + godot::String::num(to.x) + ", " + godot::String::num(to.z) +
      ") distance: " + godot::String::num(distance) +
      " width: " + godot::String::num(width) + " [BLUE LINE]");
}

void SkillshotDebugRenderer::show_aoe_explosion(const Vector3& center,
                                                float radius,
                                                float lifetime) {
  if (!debug_visuals_enabled) {
    return;
  }

  UtilityFunctions::print(
      "[SkillshotDebugRenderer] AoE explosion at (" +
      godot::String::num(center.x) + ", " + godot::String::num(center.z) +
      ") radius: " + godot::String::num(radius) +
      " lifetime: " + godot::String::num(lifetime) + "s [GREEN CIRCLE]");
}

void SkillshotDebugRenderer::show_unit_hit_indicator(const Vector3& point,
                                                     float lifetime) {
  if (!debug_visuals_enabled) {
    return;
  }

  UtilityFunctions::print("[SkillshotDebugRenderer] Unit hit at (" +
                          godot::String::num(point.x) + ", " +
                          godot::String::num(point.z) + ") [RED X]");
}

void SkillshotDebugRenderer::set_debug_visuals_enabled(bool enabled) {
  debug_visuals_enabled = enabled;
  UtilityFunctions::print("[SkillshotDebugRenderer] Debug visuals " +
                          godot::String(enabled ? "enabled" : "disabled"));
}

bool SkillshotDebugRenderer::is_debug_visuals_enabled() const {
  return debug_visuals_enabled;
}

SkillshotDebugRenderer* SkillshotDebugRenderer::get_singleton() {
  return singleton_instance;
}

Node3D* SkillshotDebugRenderer::create_debug_sphere(const Vector3& center,
                                                    float radius) {
  MeshInstance3D* sphere = memnew(MeshInstance3D);
  Ref<SphereMesh> mesh = Ref<SphereMesh>(memnew(SphereMesh));
  mesh->set_radius(radius);
  mesh->set_height(radius * 2.0f);
  sphere->set_mesh(mesh);
  sphere->set_global_position(center);

  Ref<StandardMaterial3D> material =
      Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
  material->set_albedo(Color(0, 1, 1, 0.5f));  // Cyan
  material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
  sphere->set_material_override(material);

  add_child(sphere);
  return sphere;
}

Node3D* SkillshotDebugRenderer::create_debug_circle(const Vector3& center,
                                                    float radius) {
  // Use a thin cylinder mesh to represent a circle
  MeshInstance3D* circle = memnew(MeshInstance3D);
  Ref<CylinderMesh> mesh = Ref<CylinderMesh>(memnew(CylinderMesh));
  mesh->set_height(0.05f);
  circle->set_mesh(mesh);
  circle->set_global_position(center);

  // Scale to create the circle effect
  circle->set_scale(Vector3(radius, 0.05f, radius));

  Ref<StandardMaterial3D> material =
      Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
  material->set_albedo(Color(0, 1, 0, 0.3f));  // Green
  material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
  circle->set_material_override(material);

  add_child(circle);
  return circle;
}

Node3D* SkillshotDebugRenderer::create_debug_line(const Vector3& from,
                                                  const Vector3& to,
                                                  float width) {
  Vector3 direction = to - from;
  float length = direction.length();

  if (length < 0.001f) {
    return nullptr;
  }

  MeshInstance3D* line = memnew(MeshInstance3D);
  Ref<BoxMesh> mesh = Ref<BoxMesh>(memnew(BoxMesh));
  mesh->set_size(Vector3(width, width, length));
  line->set_mesh(mesh);

  Vector3 center = (from + to) * 0.5f;
  line->set_global_position(center);

  // Rotate to point from->to
  Vector3 normalized_dir = direction / length;
  line->look_at(to, Vector3(0, 1, 0));

  Ref<StandardMaterial3D> material =
      Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
  material->set_albedo(Color(1, 1, 0, 0.8f));  // Yellow
  line->set_material_override(material);

  add_child(line);
  return line;
}

void SkillshotDebugRenderer::cleanup_old_visuals() {
  // Remove expired debug visuals
  for (int i = get_child_count() - 1; i >= 0; i--) {
    // Could track lifetime and remove here
  }
}
