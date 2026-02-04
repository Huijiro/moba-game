#ifndef GDEXTENSION_DEBUG_UTILS_H
#define GDEXTENSION_DEBUG_UTILS_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include "debug_logger.hpp"
#include "debug_macros.hpp"
#include "visual_debugger.hpp"

using godot::Color;
using godot::Node3D;
using godot::String;
using godot::Vector3;

namespace DebugUtils {

/// Draw a path showing movement trajectory
inline void draw_path(const Vector3* points,
                      int point_count,
                      float point_radius,
                      const Color& color = Color(0, 1, 0, 1),
                      float thickness = 1.0f) {
  auto* debugger = VisualDebugger::get_singleton();
  if (!debugger || !debugger->is_debug_enabled()) {
    return;
  }

  for (int i = 0; i < point_count; ++i) {
    debugger->draw_sphere(points[i], point_radius, color, 8, thickness);

    if (i < point_count - 1) {
      debugger->draw_line(points[i], points[i + 1], color, thickness);
    }
  }
}

/// Draw a unit's position and facing direction
inline void draw_unit_debug(Node3D* unit,
                            const Vector3& forward_direction,
                            float debug_size = 0.5f) {
  auto* debugger = VisualDebugger::get_singleton();
  if (!debugger || !debugger->is_debug_enabled()) {
    return;
  }

  if (!unit) {
    return;
  }

  Vector3 unit_pos = unit->get_global_position();

  // Draw cross at unit position
  debugger->draw_cross(unit_pos, debug_size, Color(1, 0, 0, 1), 1.0f);

  // Draw forward direction as vector
  debugger->draw_vector(unit_pos, forward_direction, debug_size * 2,
                        Color(0, 1, 0, 1), 1.0f);

  // Draw velocity or movement speed indicator
  debugger->draw_circle_xz(unit_pos, debug_size, Color(0, 0, 1, 0.5f), 16, 1.0f,
                           false);
}

/// Draw a range indicator around a target position
inline void draw_range_indicator(const Vector3& center,
                                 float range,
                                 const Color& color = Color(1, 1, 0, 0.3f),
                                 float thickness = 1.0f) {
  auto* debugger = VisualDebugger::get_singleton();
  if (!debugger || !debugger->is_debug_enabled()) {
    return;
  }

  debugger->draw_circle_xz(center, range, color, 32, thickness, false);
}

/// Draw an attack range indicator
inline void draw_attack_range(const Vector3& center, float attack_range) {
  draw_range_indicator(center, attack_range, Color(1, 0, 0, 0.2f), 2.0f);
}

/// Draw a detection/vision range
inline void draw_vision_range(const Vector3& center, float vision_range) {
  draw_range_indicator(center, vision_range, Color(0, 1, 1, 0.1f), 1.0f);
}

/// Log debug information about a Unit
inline void log_unit_state(const String& category,
                           Node3D* unit,
                           const String& additional_info = "") {
  if (!unit) {
    DBG_ERROR(category, "Attempted to log null unit");
    return;
  }

  Vector3 pos = unit->get_global_position();
  String unit_name(unit->get_name());
  String msg = unit_name + String(" at (") + String::num(pos.x) + String(", ") +
               String::num(pos.y) + String(", ") + String::num(pos.z) +
               String(")");

  if (!additional_info.is_empty()) {
    msg = msg + String(" | ") + additional_info;
  }

  auto* logger = DebugLogger::get_singleton();
  if (logger) {
    logger->debug(category, msg);
  }
}

/// Assert with detailed logging
inline void assert_with_log(bool condition,
                            const String& category,
                            const String& assertion_name,
                            const String& failure_message) {
  if (!condition) {
    String msg = String("ASSERTION FAILED: ") + assertion_name + String(" - ") +
                 failure_message;
    auto* logger = DebugLogger::get_singleton();
    if (logger) {
      logger->error(category, msg);
    }
  }
}

/// Time a function or code block execution (use with RAII pattern)
class ScopedTimer {
 private:
  String category;
  String name;
  double start_time;

 public:
  ScopedTimer(const String& cat, const String& timer_name)
      : category(cat), name(timer_name) {
    // Note: Would need proper timer implementation with OS::get_ticks_msec()
    start_time = 0;
  }

  ~ScopedTimer() {
    // Log elapsed time when destroyed
    // double elapsed = OS::get_ticks_msec() - start_time;
    // DebugLogger::debug(category, name + " took " +
    // String::num(elapsed) + "ms");
  }
};

}  // namespace DebugUtils

#endif  // GDEXTENSION_DEBUG_UTILS_H
