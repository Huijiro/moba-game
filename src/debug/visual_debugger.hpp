#ifndef GDEXTENSION_VISUAL_DEBUGGER_H
#define GDEXTENSION_VISUAL_DEBUGGER_H

#include <godot_cpp/classes/immediate_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <vector>

using godot::Color;
using godot::ImmediateMesh;
using godot::MeshInstance3D;
using godot::Node3D;
using godot::Ref;
using godot::Vector3;

enum class DrawType {
  LINE,
  CIRCLE,
};

struct PendingDraw {
  DrawType type;
  Vector3 p1;
  Vector3 p2;
  Vector3 center;       // For circles
  float radius = 0.0f;  // For circles
  Color color;
  float thickness = 1.0f;  // Line thickness in units
  bool filled = false;     // Whether to fill shapes
  int segments = 32;       // For circles
};

/// Visual debugger using ImmediateMesh for real-time drawing
/// Draws immediately to viewport without creating persistent objects
class VisualDebugger : public Node3D {
  GDCLASS(VisualDebugger, Node3D)

 protected:
  static void _bind_methods();

  bool debug_enabled = true;
  MeshInstance3D* mesh_instance = nullptr;
  Ref<ImmediateMesh> immediate_mesh = nullptr;
  std::vector<PendingDraw> pending_draws;

 public:
  VisualDebugger();
  ~VisualDebugger();

  void _ready() override;
  void _process(double delta) override;

  // Draw functions - queue up draws for flushing at end of frame
  void draw_circle_xz(const Vector3& center,
                      float radius,
                      const Color& color = Color(0, 1, 0, 1),
                      int segments = 32,
                      float thickness = 1.0f,
                      bool filled = false);

  void draw_box(const Vector3& center,
                const Vector3& size,
                const Color& color = Color(1, 1, 1, 1),
                float thickness = 1.0f,
                bool filled = false);

  void draw_line(const Vector3& from,
                 const Vector3& to,
                 const Color& color = Color(1, 1, 1, 1),
                 float thickness = 1.0f);

  void clear();

  void set_debug_enabled(bool enabled);
  bool is_debug_enabled() const;

  static VisualDebugger* get_singleton();

 private:
  static VisualDebugger* singleton_instance;

  void _flush_draws();
};

#endif  // GDEXTENSION_VISUAL_DEBUGGER_H
