#include "debug_mesh_renderer.hpp"

#include <cmath>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/capsule_mesh.hpp>
#include <godot_cpp/classes/cylinder_mesh.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::BoxMesh;
using godot::CapsuleMesh;
using godot::ClassDB;
using godot::CylinderMesh;
using godot::D_METHOD;
using godot::Engine;
using godot::Object;
using godot::PropertyInfo;
using godot::Ref;
using godot::SphereMesh;
using godot::UtilityFunctions;
using godot::Variant;

DebugMeshRenderer* DebugMeshRenderer::singleton_instance = nullptr;

DebugMeshRenderer::DebugMeshRenderer() = default;

DebugMeshRenderer::~DebugMeshRenderer() = default;

void DebugMeshRenderer::_bind_methods() {
  ClassDB::bind_method(
      D_METHOD("draw_circle", "center", "radius", "color", "lifetime"),
      &DebugMeshRenderer::draw_circle, DEFVAL(Color(0, 1, 0, 0.3f)),
      DEFVAL(0.3f));
  ClassDB::bind_method(
      D_METHOD("draw_point", "position", "size", "color", "lifetime"),
      &DebugMeshRenderer::draw_point, DEFVAL(0.3f), DEFVAL(Color(1, 0, 0, 1)),
      DEFVAL(0.3f));
  ClassDB::bind_method(
      D_METHOD("draw_arrow", "from", "to", "color", "lifetime"),
      &DebugMeshRenderer::draw_arrow, DEFVAL(Color(1, 1, 0, 1)), DEFVAL(0.3f));

  ClassDB::bind_method(D_METHOD("set_debug_enabled", "enabled"),
                       &DebugMeshRenderer::set_debug_enabled);
  ClassDB::bind_method(D_METHOD("is_debug_enabled"),
                       &DebugMeshRenderer::is_debug_enabled);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_enabled"),
               "set_debug_enabled", "is_debug_enabled");
}

void DebugMeshRenderer::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (singleton_instance == nullptr) {
    singleton_instance = this;
    UtilityFunctions::print("[DebugMeshRenderer] Initialized as singleton");
  }

  set_name("DebugMeshRenderer");
}

void DebugMeshRenderer::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  cleanup_expired_meshes();
}

void DebugMeshRenderer::draw_circle(const Vector3& center,
                                    float radius,
                                    const Color& color,
                                    float lifetime) {
  if (!debug_enabled) {
    return;
  }

  // Create a thin cylinder to represent a circle
  MeshInstance3D* circle = memnew(MeshInstance3D);
  Ref<CylinderMesh> mesh = Ref<CylinderMesh>(memnew(CylinderMesh));
  mesh->set_height(0.05f);
  circle->set_mesh(mesh);
  circle->set_global_position(center);
  circle->set_scale(Vector3(radius, 0.05f, radius));

  Ref<StandardMaterial3D> material =
      Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
  material->set_albedo(color);
  material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
  circle->set_material_override(material);

  add_child(circle);

  DebugMesh dm;
  dm.mesh = circle;
  dm.lifetime_remaining = lifetime;
  active_meshes.push(dm);

  UtilityFunctions::print(
      "[DebugMeshRenderer] Circle at (" + godot::String::num(center.x) + ", " +
      godot::String::num(center.z) + ") radius " + godot::String::num(radius));
}

void DebugMeshRenderer::draw_point(const Vector3& position,
                                   float size,
                                   const Color& color,
                                   float lifetime) {
  if (!debug_enabled) {
    return;
  }

  MeshInstance3D* point = memnew(MeshInstance3D);
  Ref<SphereMesh> mesh = Ref<SphereMesh>(memnew(SphereMesh));
  mesh->set_radius(size * 0.5f);
  mesh->set_height(size);
  point->set_mesh(mesh);
  point->set_global_position(position);

  Ref<StandardMaterial3D> material =
      Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
  material->set_albedo(color);
  material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
  point->set_material_override(material);

  add_child(point);

  DebugMesh dm;
  dm.mesh = point;
  dm.lifetime_remaining = lifetime;
  active_meshes.push(dm);

  UtilityFunctions::print("[DebugMeshRenderer] Point at (" +
                          godot::String::num(position.x) + ", " +
                          godot::String::num(position.z) + ")");
}

void DebugMeshRenderer::draw_arrow(const Vector3& from,
                                   const Vector3& to,
                                   const Color& color,
                                   float lifetime) {
  if (!debug_enabled) {
    return;
  }

  Vector3 direction = to - from;
  float length = direction.length();

  if (length < 0.001f) {
    return;
  }

  // Draw the line
  MeshInstance3D* line = memnew(MeshInstance3D);
  Ref<CylinderMesh> mesh = Ref<CylinderMesh>(memnew(CylinderMesh));
  mesh->set_height(length);
  line->set_mesh(mesh);

  Vector3 center = (from + to) * 0.5f;
  line->set_global_position(center);
  line->set_scale(Vector3(0.1f, 1.0f, 0.1f));

  // Rotate to point from->to
  line->look_at(to, Vector3(0, 1, 0));

  Ref<StandardMaterial3D> material =
      Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
  material->set_albedo(color);
  material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
  line->set_material_override(material);

  add_child(line);

  DebugMesh dm;
  dm.mesh = line;
  dm.lifetime_remaining = lifetime;
  active_meshes.push(dm);

  UtilityFunctions::print(
      "[DebugMeshRenderer] Arrow from (" + godot::String::num(from.x) + ", " +
      godot::String::num(from.z) + ") to (" + godot::String::num(to.x) + ", " +
      godot::String::num(to.z) + ")");
}

void DebugMeshRenderer::set_debug_enabled(bool enabled) {
  debug_enabled = enabled;
}

bool DebugMeshRenderer::is_debug_enabled() const {
  return debug_enabled;
}

DebugMeshRenderer* DebugMeshRenderer::get_singleton() {
  return singleton_instance;
}

void DebugMeshRenderer::cleanup_expired_meshes() {
  while (!active_meshes.empty()) {
    DebugMesh& dm = active_meshes.front();
    dm.lifetime_remaining -= get_physics_process_delta_time();

    if (dm.lifetime_remaining <= 0.0f) {
      if (dm.mesh != nullptr && dm.mesh->is_inside_tree()) {
        dm.mesh->queue_free();
      }
      active_meshes.pop();
    } else {
      break;  // Since queue is FIFO, if this one isn't expired, rest aren't
              // either
    }
  }
}
