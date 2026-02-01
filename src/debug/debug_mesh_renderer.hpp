#ifndef GDEXTENSION_DEBUG_MESH_RENDERER_H
#define GDEXTENSION_DEBUG_MESH_RENDERER_H

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <queue>

using godot::Color;
using godot::MeshInstance3D;
using godot::Node3D;
using godot::Ref;
using godot::StandardMaterial3D;
using godot::Vector3;

/// Simple debug mesh renderer that creates temporary mesh instances in the
/// viewport These meshes automatically clean up after a short duration
class DebugMeshRenderer : public Node3D {
  GDCLASS(DebugMeshRenderer, Node3D)

 protected:
  static void _bind_methods();

  bool debug_enabled = true;
  float default_lifetime = 0.1f;
  float mesh_creation_cooldown = 0.0f;  // Cooldown before creating next mesh
  float min_mesh_creation_interval = 0.05f;  // Min time between mesh creations

  struct DebugMesh {
    MeshInstance3D* mesh;
    float lifetime_remaining;
  };

  std::queue<DebugMesh> active_meshes;

 public:
  DebugMeshRenderer();
  ~DebugMeshRenderer();

  void _ready() override;
  void _physics_process(double delta) override;

  // Draw a circle in the XZ plane
  void draw_circle(const Vector3& center,
                   float radius,
                   const Color& color = Color(0, 1, 0, 0.3f),
                   float lifetime = 0.3f);

  // Draw a point/sphere
  void draw_point(const Vector3& position,
                  float size = 0.3f,
                  const Color& color = Color(1, 0, 0, 1),
                  float lifetime = 0.3f);

  // Draw an arrow from->to
  void draw_arrow(const Vector3& from,
                  const Vector3& to,
                  const Color& color = Color(1, 1, 0, 1),
                  float lifetime = 0.3f);

  void set_debug_enabled(bool enabled);
  bool is_debug_enabled() const;

  void set_min_mesh_creation_interval(float interval);
  float get_min_mesh_creation_interval() const;

  void clear_all_meshes();

  static DebugMeshRenderer* get_singleton();

 private:
  static DebugMeshRenderer* singleton_instance;

  void cleanup_expired_meshes();
};

#endif  // GDEXTENSION_DEBUG_MESH_RENDERER_H
