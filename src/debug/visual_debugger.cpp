#include "visual_debugger.hpp"

#include <cmath>
#include <godot_cpp/classes/tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::Mesh;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

VisualDebugger* VisualDebugger::singleton_instance = nullptr;

VisualDebugger::VisualDebugger() {}

VisualDebugger::~VisualDebugger() {
  if (singleton_instance == this) {
    singleton_instance = nullptr;
  }
}

void VisualDebugger::_bind_methods() {
  ClassDB::bind_method(
      D_METHOD("draw_circle_xz", "center", "radius", "color", "segments"),
      &VisualDebugger::draw_circle_xz);

  ClassDB::bind_method(D_METHOD("draw_box", "center", "size", "color"),
                       &VisualDebugger::draw_box);

  ClassDB::bind_method(D_METHOD("draw_line", "from", "to", "color"),
                       &VisualDebugger::draw_line);

  ClassDB::bind_method(D_METHOD("clear"), &VisualDebugger::clear);

  ClassDB::bind_method(D_METHOD("set_debug_enabled", "enabled"),
                       &VisualDebugger::set_debug_enabled);

  ClassDB::bind_method(D_METHOD("is_debug_enabled"),
                       &VisualDebugger::is_debug_enabled);

  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_enabled"),
               "set_debug_enabled", "is_debug_enabled");
}

void VisualDebugger::_ready() {
  // Create MeshInstance3D if it doesn't exist
  if (!mesh_instance) {
    mesh_instance = memnew(MeshInstance3D);
    add_child(mesh_instance);
  }

  // Create ImmediateMesh
  if (!immediate_mesh.is_valid()) {
    immediate_mesh.instantiate();
    mesh_instance->set_mesh(immediate_mesh);
  }

  // Set singleton
  singleton_instance = this;
  UtilityFunctions::print("[VisualDebugger] Ready - singleton initialized");
}

void VisualDebugger::_process(double delta) {
  if (!debug_enabled || !immediate_mesh.is_valid()) {
    return;
  }

  // Flush pending draws at end of frame
  _flush_draws();
}

void VisualDebugger::clear() {
  pending_draws.clear();
}

void VisualDebugger::draw_circle_xz(const Vector3& center,
                                    float radius,
                                    const Color& color,
                                    int segments) {
  if (!debug_enabled) {
    return;
  }

  float angle_step = 2.0f * 3.14159265f / segments;

  for (int i = 0; i < segments; i++) {
    float angle1 = i * angle_step;
    float angle2 = (i + 1) * angle_step;

    // Calculate points on circle in XZ plane
    float x1 = center.x + radius * std::cos(angle1);
    float z1 = center.z + radius * std::sin(angle1);
    float x2 = center.x + radius * std::cos(angle2);
    float z2 = center.z + radius * std::sin(angle2);

    Vector3 p1(x1, center.y, z1);
    Vector3 p2(x2, center.y, z2);

    // Add line segment to pending draws
    PendingDraw draw;
    draw.type = DrawType::LINE;
    draw.p1 = p1;
    draw.p2 = p2;
    draw.color = color;
    pending_draws.push_back(draw);
  }
}

void VisualDebugger::draw_box(const Vector3& center,
                              const Vector3& size,
                              const Color& color) {
  if (!debug_enabled) {
    return;
  }

  // Calculate corners
  Vector3 half_size = size * 0.5f;
  Vector3 corners[8] = {
      center + Vector3(-half_size.x, -half_size.y, -half_size.z),
      center + Vector3(half_size.x, -half_size.y, -half_size.z),
      center + Vector3(half_size.x, half_size.y, -half_size.z),
      center + Vector3(-half_size.x, half_size.y, -half_size.z),
      center + Vector3(-half_size.x, -half_size.y, half_size.z),
      center + Vector3(half_size.x, -half_size.y, half_size.z),
      center + Vector3(half_size.x, half_size.y, half_size.z),
      center + Vector3(-half_size.x, half_size.y, half_size.z),
  };

  // Draw box edges
  int edges[12][2] = {
      {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Front face
      {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Back face
      {0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connecting edges
  };

  for (int i = 0; i < 12; i++) {
    PendingDraw draw;
    draw.type = DrawType::LINE;
    draw.p1 = corners[edges[i][0]];
    draw.p2 = corners[edges[i][1]];
    draw.color = color;
    pending_draws.push_back(draw);
  }
}

void VisualDebugger::draw_line(const Vector3& from,
                               const Vector3& to,
                               const Color& color) {
  if (!debug_enabled) {
    return;
  }

  PendingDraw draw;
  draw.type = DrawType::LINE;
  draw.p1 = from;
  draw.p2 = to;
  draw.color = color;
  pending_draws.push_back(draw);
}

void VisualDebugger::_flush_draws() {
  if (!immediate_mesh.is_valid() || pending_draws.empty()) {
    immediate_mesh->clear_surfaces();
    return;
  }

  // Clear previous frame
  immediate_mesh->clear_surfaces();

  // Begin new surface
  immediate_mesh->surface_begin(Mesh::PRIMITIVE_LINES);

  // Add all pending draws
  for (const auto& draw : pending_draws) {
    immediate_mesh->surface_set_color(draw.color);
    immediate_mesh->surface_add_vertex(draw.p1);
    immediate_mesh->surface_add_vertex(draw.p2);
  }

  // End surface
  immediate_mesh->surface_end();

  // Clear for next frame
  pending_draws.clear();
}

void VisualDebugger::set_debug_enabled(bool enabled) {
  debug_enabled = enabled;
  if (mesh_instance) {
    mesh_instance->set_visible(enabled);
  }
  if (!enabled) {
    pending_draws.clear();
  }
}

bool VisualDebugger::is_debug_enabled() const {
  return debug_enabled;
}

VisualDebugger* VisualDebugger::get_singleton() {
  return singleton_instance;
}
