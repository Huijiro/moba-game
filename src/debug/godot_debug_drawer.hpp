#ifndef GDEXTENSION_GODOT_DEBUG_DRAWER_H
#define GDEXTENSION_GODOT_DEBUG_DRAWER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Color;
using godot::Node3D;
using godot::Vector3;

/// Debug drawer using Godot's built-in debug_draw_* methods
/// Draws directly to the viewport without creating persistent meshes
/// Used to visualize ability ranges, areas, and targets during development
class GodotDebugDrawer : public Node3D {
  GDCLASS(GodotDebugDrawer, Node3D)

 protected:
  static void _bind_methods();

  bool debug_enabled = true;
  bool draw_ability_ranges = true;
  bool draw_targeting_areas = true;
  bool draw_unit_targets = true;

 public:
  GodotDebugDrawer();
  ~GodotDebugDrawer();

  void _physics_process(double delta) override;

  // Debug drawing methods using Godot's built-in tools
  void draw_circle_xz(const Vector3& center,
                      float radius,
                      const Color& color = Color(0, 1, 0, 1),
                      int segments = 32);

  void draw_line(const Vector3& from,
                 const Vector3& to,
                 const Color& color = Color(1, 1, 1, 1),
                 float width = 0.02f);

  void draw_sphere(const Vector3& center,
                   float radius,
                   const Color& color = Color(1, 1, 1, 1),
                   int segments = 16);

  void draw_box(const Vector3& center,
                const Vector3& extents,
                const Color& color = Color(1, 1, 1, 1));

  void draw_cross(const Vector3& position,
                  float size = 0.5f,
                  const Color& color = Color(1, 0, 0, 1));

  void draw_arrow(const Vector3& from,
                  const Vector3& to,
                  const Color& color = Color(1, 1, 0, 1),
                  float head_size = 0.3f);

  // Ability-specific debug visualization
  void draw_ability_range(const Vector3& from_unit,
                          float range,
                          const Color& color = Color(0, 0.5f, 1, 0.3f));

  void draw_ability_area(const Vector3& center,
                         float radius,
                         const Color& color = Color(0, 1, 0, 0.4f));

  void draw_unit_target(const Vector3& target_pos,
                        const Color& color = Color(1, 0.5f, 0, 1));

  void draw_projectile_path(const Vector3& from,
                            const Vector3& direction,
                            float distance,
                            const Color& color = Color(0, 1, 1, 0.5f));

  // Control
  void set_debug_enabled(bool enabled);
  bool is_debug_enabled() const;

  void set_draw_ability_ranges(bool enabled);
  bool is_draw_ability_ranges() const;

  void set_draw_targeting_areas(bool enabled);
  bool is_draw_targeting_areas() const;

  void set_draw_unit_targets(bool enabled);
  bool is_draw_unit_targets() const;

  // Singleton access
  static GodotDebugDrawer* get_singleton();

 private:
  static GodotDebugDrawer* singleton_instance;
};

#endif  // GDEXTENSION_GODOT_DEBUG_DRAWER_H
