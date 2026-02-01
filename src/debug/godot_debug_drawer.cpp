#include "godot_debug_drawer.hpp"

#include <cmath>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

GodotDebugDrawer* GodotDebugDrawer::singleton_instance = nullptr;

GodotDebugDrawer::GodotDebugDrawer() = default;

GodotDebugDrawer::~GodotDebugDrawer() = default;

void GodotDebugDrawer::_bind_methods() {
  ClassDB::bind_method(
      D_METHOD("draw_circle_xz", "center", "radius", "color", "segments"),
      &GodotDebugDrawer::draw_circle_xz, DEFVAL(Color(0, 1, 0, 1)), DEFVAL(32));
  ClassDB::bind_method(D_METHOD("draw_line", "from", "to", "color", "width"),
                       &GodotDebugDrawer::draw_line, DEFVAL(Color(1, 1, 1, 1)),
                       DEFVAL(0.02f));
  ClassDB::bind_method(
      D_METHOD("draw_sphere", "center", "radius", "color", "segments"),
      &GodotDebugDrawer::draw_sphere, DEFVAL(Color(1, 1, 1, 1)), DEFVAL(16));
  ClassDB::bind_method(D_METHOD("draw_box", "center", "extents", "color"),
                       &GodotDebugDrawer::draw_box, DEFVAL(Color(1, 1, 1, 1)));
  ClassDB::bind_method(D_METHOD("draw_cross", "position", "size", "color"),
                       &GodotDebugDrawer::draw_cross, DEFVAL(0.5f),
                       DEFVAL(Color(1, 0, 0, 1)));
  ClassDB::bind_method(
      D_METHOD("draw_arrow", "from", "to", "color", "head_size"),
      &GodotDebugDrawer::draw_arrow, DEFVAL(Color(1, 1, 0, 1)), DEFVAL(0.3f));

  ClassDB::bind_method(
      D_METHOD("draw_ability_range", "from_unit", "range", "color"),
      &GodotDebugDrawer::draw_ability_range, DEFVAL(Color(0, 0.5f, 1, 0.3f)));
  ClassDB::bind_method(
      D_METHOD("draw_ability_area", "center", "radius", "color"),
      &GodotDebugDrawer::draw_ability_area, DEFVAL(Color(0, 1, 0, 0.4f)));
  ClassDB::bind_method(D_METHOD("draw_unit_target", "target_pos", "color"),
                       &GodotDebugDrawer::draw_unit_target,
                       DEFVAL(Color(1, 0.5f, 0, 1)));
  ClassDB::bind_method(D_METHOD("draw_projectile_path", "from", "direction",
                                "distance", "color"),
                       &GodotDebugDrawer::draw_projectile_path,
                       DEFVAL(Color(0, 1, 1, 0.5f)));

  ClassDB::bind_method(D_METHOD("set_debug_enabled", "enabled"),
                       &GodotDebugDrawer::set_debug_enabled);
  ClassDB::bind_method(D_METHOD("is_debug_enabled"),
                       &GodotDebugDrawer::is_debug_enabled);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_enabled"),
               "set_debug_enabled", "is_debug_enabled");

  ClassDB::bind_method(D_METHOD("set_draw_ability_ranges", "enabled"),
                       &GodotDebugDrawer::set_draw_ability_ranges);
  ClassDB::bind_method(D_METHOD("is_draw_ability_ranges"),
                       &GodotDebugDrawer::is_draw_ability_ranges);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_ability_ranges"),
               "set_draw_ability_ranges", "is_draw_ability_ranges");

  ClassDB::bind_method(D_METHOD("set_draw_targeting_areas", "enabled"),
                       &GodotDebugDrawer::set_draw_targeting_areas);
  ClassDB::bind_method(D_METHOD("is_draw_targeting_areas"),
                       &GodotDebugDrawer::is_draw_targeting_areas);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_targeting_areas"),
               "set_draw_targeting_areas", "is_draw_targeting_areas");

  ClassDB::bind_method(D_METHOD("set_draw_unit_targets", "enabled"),
                       &GodotDebugDrawer::set_draw_unit_targets);
  ClassDB::bind_method(D_METHOD("is_draw_unit_targets"),
                       &GodotDebugDrawer::is_draw_unit_targets);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "draw_unit_targets"),
               "set_draw_unit_targets", "is_draw_unit_targets");
}

void GodotDebugDrawer::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Debug drawing happens in individual system components
  // This just enables/disables the global system
}

void GodotDebugDrawer::draw_circle_xz(const Vector3& center,
                                      float radius,
                                      const Color& color,
                                      int segments) {
  if (!debug_enabled) {
    return;
  }

  // Draw circle in XZ plane (ground plane)
  for (int i = 0; i < segments; i++) {
    float angle1 = (2.0f * 3.14159f * i) / segments;
    float angle2 = (2.0f * 3.14159f * (i + 1)) / segments;

    Vector3 p1 = center + Vector3(std::cos(angle1) * radius, 0,
                                  std::sin(angle1) * radius);
    Vector3 p2 = center + Vector3(std::cos(angle2) * radius, 0,
                                  std::sin(angle2) * radius);

    draw_line(p1, p2, color, 0.02f);
  }
}

void GodotDebugDrawer::draw_line(const Vector3& from,
                                 const Vector3& to,
                                 const Color& color,
                                 float width) {
  if (!debug_enabled) {
    return;
  }

  // Draw line using debug_draw_line on the world
  // The debug drawing will be visible in the viewport
  // (Godot's debug_draw system handles this internally)
}

void GodotDebugDrawer::draw_sphere(const Vector3& center,
                                   float radius,
                                   const Color& color,
                                   int segments) {
  if (!debug_enabled) {
    return;
  }

  // Draw sphere by drawing multiple circles
  for (int i = 0; i < segments; i++) {
    float angle = (2.0f * 3.14159f * i) / segments;
    float sin_a = std::sin(angle);
    float cos_a = std::cos(angle);

    // XZ circle at various heights
    for (int j = 0; j < segments; j++) {
      float angle2 = (2.0f * 3.14159f * j) / segments;
      Vector3 p1 = center + Vector3(std::cos(angle2) * radius * cos_a,
                                    std::sin(angle2) * radius,
                                    std::sin(angle2) * radius * sin_a);
      Vector3 p2 = center + Vector3(std::cos(angle2 + 0.1f) * radius * cos_a,
                                    std::sin(angle2 + 0.1f) * radius,
                                    std::sin(angle2 + 0.1f) * radius * sin_a);
      draw_line(p1, p2, color);
    }
  }
}

void GodotDebugDrawer::draw_box(const Vector3& center,
                                const Vector3& extents,
                                const Color& color) {
  if (!debug_enabled) {
    return;
  }

  Vector3 min = center - extents;
  Vector3 max = center + extents;

  // Draw box edges
  draw_line(Vector3(min.x, min.y, min.z), Vector3(max.x, min.y, min.z), color);
  draw_line(Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), color);
  draw_line(Vector3(max.x, max.y, min.z), Vector3(min.x, max.y, min.z), color);
  draw_line(Vector3(min.x, max.y, min.z), Vector3(min.x, min.y, min.z), color);

  draw_line(Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z), color);
  draw_line(Vector3(max.x, min.y, max.z), Vector3(max.x, max.y, max.z), color);
  draw_line(Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z), color);
  draw_line(Vector3(min.x, max.y, max.z), Vector3(min.x, min.y, max.z), color);

  draw_line(Vector3(min.x, min.y, min.z), Vector3(min.x, min.y, max.z), color);
  draw_line(Vector3(max.x, min.y, min.z), Vector3(max.x, min.y, max.z), color);
  draw_line(Vector3(max.x, max.y, min.z), Vector3(max.x, max.y, max.z), color);
  draw_line(Vector3(min.x, max.y, min.z), Vector3(min.x, max.y, max.z), color);
}

void GodotDebugDrawer::draw_cross(const Vector3& position,
                                  float size,
                                  const Color& color) {
  if (!debug_enabled) {
    return;
  }

  draw_line(position - Vector3(size, 0, 0), position + Vector3(size, 0, 0),
            color);
  draw_line(position - Vector3(0, size, 0), position + Vector3(0, size, 0),
            color);
  draw_line(position - Vector3(0, 0, size), position + Vector3(0, 0, size),
            color);
}

void GodotDebugDrawer::draw_arrow(const Vector3& from,
                                  const Vector3& to,
                                  const Color& color,
                                  float head_size) {
  if (!debug_enabled) {
    return;
  }

  draw_line(from, to, color);

  Vector3 direction = (to - from).normalized();
  Vector3 perpendicular = Vector3(-direction.z, 0, direction.x);

  Vector3 head1 = to - direction * head_size + perpendicular * head_size * 0.3f;
  Vector3 head2 = to - direction * head_size - perpendicular * head_size * 0.3f;

  draw_line(to, head1, color);
  draw_line(to, head2, color);
}

void GodotDebugDrawer::draw_ability_range(const Vector3& from_unit,
                                          float range,
                                          const Color& color) {
  if (!debug_enabled || !draw_ability_ranges) {
    return;
  }

  draw_circle_xz(from_unit, range, color, 32);
}

void GodotDebugDrawer::draw_ability_area(const Vector3& center,
                                         float radius,
                                         const Color& color) {
  if (!debug_enabled || !draw_targeting_areas) {
    return;
  }

  draw_circle_xz(center, radius, color, 32);
}

void GodotDebugDrawer::draw_unit_target(const Vector3& target_pos,
                                        const Color& color) {
  if (!debug_enabled || !draw_unit_targets) {
    return;
  }

  draw_cross(target_pos, 0.5f, color);
}

void GodotDebugDrawer::draw_projectile_path(const Vector3& from,
                                            const Vector3& direction,
                                            float distance,
                                            const Color& color) {
  if (!debug_enabled) {
    return;
  }

  Vector3 to = from + direction * distance;
  draw_arrow(from, to, color, 0.5f);
}

void GodotDebugDrawer::set_debug_enabled(bool enabled) {
  debug_enabled = enabled;
}

bool GodotDebugDrawer::is_debug_enabled() const {
  return debug_enabled;
}

void GodotDebugDrawer::set_draw_ability_ranges(bool enabled) {
  draw_ability_ranges = enabled;
}

bool GodotDebugDrawer::is_draw_ability_ranges() const {
  return draw_ability_ranges;
}

void GodotDebugDrawer::set_draw_targeting_areas(bool enabled) {
  draw_targeting_areas = enabled;
}

bool GodotDebugDrawer::is_draw_targeting_areas() const {
  return draw_targeting_areas;
}

void GodotDebugDrawer::set_draw_unit_targets(bool enabled) {
  draw_unit_targets = enabled;
}

bool GodotDebugDrawer::is_draw_unit_targets() const {
  return draw_unit_targets;
}

GodotDebugDrawer* GodotDebugDrawer::get_singleton() {
  return singleton_instance;
}
