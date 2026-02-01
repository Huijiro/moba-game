#ifndef GDEXTENSION_VISUAL_DEBUGGER_H
#define GDEXTENSION_VISUAL_DEBUGGER_H

#include <godot_cpp/classes/immediate_mesh.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Color;
using godot::ImmediateMesh;
using godot::MeshInstance3D;
using godot::Node3D;
using godot::Ref;
using godot::Vector3;

/// Visual debugger using ImmediateMesh for real-time drawing
/// Draws immediately to viewport without creating persistent objects
class VisualDebugger : public Node3D {
  GDCLASS(VisualDebugger, Node3D)

 protected:
  static void _bind_methods();

  bool debug_enabled = true;
  MeshInstance3D* mesh_instance = nullptr;
  Ref<ImmediateMesh> immediate_mesh = nullptr;

 public:
  VisualDebugger();
  ~VisualDebugger();

  void _ready() override;
  void _process(double delta) override;

  // Draw functions
  void draw_circle_xz(const Vector3& center,
                      float radius,
                      const Color& color = Color(0, 1, 0, 1),
                      int segments = 32);

  void draw_box(const Vector3& center,
                const Vector3& size,
                const Color& color = Color(1, 1, 1, 1));

  void draw_line(const Vector3& from,
                 const Vector3& to,
                 const Color& color = Color(1, 1, 1, 1));

  void clear();

  void set_debug_enabled(bool enabled);
  bool is_debug_enabled() const;

  static VisualDebugger* get_singleton();

 private:
  static VisualDebugger* singleton_instance;

  void begin_rendering();
  void end_rendering();
};

#endif  // GDEXTENSION_VISUAL_DEBUGGER_H
