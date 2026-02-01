#ifndef GDEXTENSION_SKILLSHOT_DEBUG_RENDERER_H
#define GDEXTENSION_SKILLSHOT_DEBUG_RENDERER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <queue>

using godot::Color;
using godot::Node3D;
using godot::Vector3;

/// Visual feedback renderer for skillshot abilities
/// Creates visual meshes and indicators for:
/// - Targeting lines while aiming
/// - Projectile trails as they travel
/// - AoE explosion radius on detonation
/// - Collision points on unit hits
class SkillshotDebugRenderer : public Node3D {
  GDCLASS(SkillshotDebugRenderer, Node3D)

 protected:
  static void _bind_methods();

  bool debug_visuals_enabled = true;

 public:
  SkillshotDebugRenderer();
  ~SkillshotDebugRenderer();

  void _ready() override;

  // Visual effects for targeting phase
  void show_targeting_line(const Vector3& from,
                           const Vector3& to,
                           float lifetime = 0.5f);

  // Visual effects during projectile flight
  void show_projectile_indicator(const Vector3& position,
                                 float radius = 0.5f,
                                 float lifetime = 0.05f);

  void show_projectile_trail(const Vector3& from,
                             const Vector3& to,
                             float width = 0.2f,
                             float lifetime = 0.1f);

  // Visual effects on detonation
  void show_aoe_explosion(const Vector3& center,
                          float radius,
                          float lifetime = 0.5f);

  void show_unit_hit_indicator(const Vector3& point, float lifetime = 0.3f);

  // Control
  void set_debug_visuals_enabled(bool enabled);
  bool is_debug_visuals_enabled() const;

  static SkillshotDebugRenderer* get_singleton();

 private:
  static SkillshotDebugRenderer* singleton_instance;

  // Helper to create visual mesh primitives
  Node3D* create_debug_sphere(const Vector3& center, float radius);
  Node3D* create_debug_circle(const Vector3& center, float radius);
  Node3D* create_debug_line(const Vector3& from,
                            const Vector3& to,
                            float width);

  // Auto-cleanup expired visuals
  void cleanup_old_visuals();
};

#endif  // GDEXTENSION_SKILLSHOT_DEBUG_RENDERER_H
