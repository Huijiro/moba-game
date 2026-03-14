#ifndef CURSOR_WORLD_QUERY_HPP
#define CURSOR_WORLD_QUERY_HPP

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/vector3.hpp>

class Unit;

/// Shared raycast utility — projects the mouse cursor into 3D world space.
///
/// Other sibling nodes (PlayerController, HoverHighlight, etc.) call
/// query() to find what's under the cursor. Caches the result per frame
/// so multiple callers don't duplicate work.
///
/// Properties:
///   camera         — Camera3D used for projection
///   controlled_unit — excluded from raycasts (player's own unit)
///   raycast_distance — max ray length
class CursorWorldQuery : public godot::Node {
  GDCLASS(CursorWorldQuery, godot::Node);

 public:
  CursorWorldQuery();
  ~CursorWorldQuery();

  void _ready() override;
  void _physics_process(double delta) override;

  /// Get the latest query result. Returns true if the ray hit something.
  bool has_hit() const;
  godot::Vector3 get_hit_position() const;
  godot::Object* get_hit_object() const;
  Unit* get_hit_unit() const;

  /// Ground-only hit position (WORLD layer only, ignores units).
  /// Use this for preview positioning.
  bool has_ground_hit() const;
  godot::Vector3 get_ground_position() const;

  void set_camera(godot::Camera3D* cam);
  godot::Camera3D* get_camera() const;

  void set_controlled_unit(Unit* unit);
  Unit* get_controlled_unit() const;

  void set_raycast_distance(float distance);
  float get_raycast_distance() const;

 protected:
  static void _bind_methods();

 private:
  godot::Camera3D* camera = nullptr;
  Unit* controlled_unit = nullptr;
  float raycast_distance = 1000.0f;

  // Cached per-frame result
  bool hit = false;
  godot::Vector3 hit_position;
  godot::Object* hit_object = nullptr;
  Unit* hit_unit = nullptr;

  bool ground_hit = false;
  godot::Vector3 ground_position;

  void _update_query();
};

#endif  // CURSOR_WORLD_QUERY_HPP
