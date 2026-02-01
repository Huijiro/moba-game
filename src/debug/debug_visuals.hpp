#ifndef GDEXTENSION_DEBUG_VISUALS_H
#define GDEXTENSION_DEBUG_VISUALS_H

#include <godot_cpp/classes/immediate_mesh.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <memory>

using godot::Color;
using godot::ImmediateMesh;
using godot::MeshInstance3D;
using godot::Node3D;
using godot::Ref;
using godot::Vector3;

/// Debug visualization system for skillshots and abilities
/// Provides visual feedback for targeting, projectiles, and AoE effects
class DebugVisuals : public Node3D {
  GDCLASS(DebugVisuals, Node3D)

 protected:
  static void _bind_methods();

  bool debug_enabled = true;
  float debug_lifetime = 0.1f;  // How long debug visuals persist

  // MeshInstance for debug draws
  MeshInstance3D* mesh_instance = nullptr;
  Ref<ImmediateMesh> immediate_mesh = nullptr;

  struct DebugPrimitive {
    float lifetime_remaining = 0.0f;
  };

 public:
  DebugVisuals();
  ~DebugVisuals();

  void _ready() override;
  void _physics_process(double delta) override;

  // Drawing functions - all coordinate in world space
  void draw_line(const Vector3& from,
                 const Vector3& to,
                 const Color& color = Color(1, 1, 1, 1),
                 float width = 0.1f);

  void draw_sphere(const Vector3& center,
                   float radius,
                   const Color& color = Color(1, 1, 1, 1),
                   int segments = 16);

  void draw_circle(const Vector3& center,
                   float radius,
                   const Color& color = Color(1, 1, 1, 1),
                   int segments = 32);

  void draw_box(const Vector3& center,
                const Vector3& size,
                const Color& color = Color(1, 1, 1, 1));

  void draw_cone(const Vector3& apex,
                 const Vector3& direction,
                 float height,
                 float base_radius,
                 const Color& color = Color(1, 1, 1, 1));

  // Convenience functions for common debug scenarios
  void draw_aoe_radius(const Vector3& center,
                       float radius,
                       const Color& color = Color(0, 1, 0, 0.3f));

  void draw_targeting_line(const Vector3& from,
                           const Vector3& to,
                           const Color& color = Color(1, 1, 0, 1));

  void draw_collision_point(const Vector3& point,
                            const Color& color = Color(1, 0, 0, 1));

  void draw_projectile_path(const Vector3& from,
                            const Vector3& direction,
                            float distance,
                            const Color& color = Color(0, 0, 1, 1));

  // Control
  void set_debug_enabled(bool enabled);
  bool is_debug_enabled() const;

  void set_debug_lifetime(float lifetime);
  float get_debug_lifetime() const;

  // Get singleton instance
  static DebugVisuals* get_singleton();

 private:
  static DebugVisuals* singleton_instance;

  void _render_frame();
};

#endif  // GDEXTENSION_DEBUG_VISUALS_H
