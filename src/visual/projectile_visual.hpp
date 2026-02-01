#ifndef GDEXTENSION_PROJECTILE_VISUAL_H
#define GDEXTENSION_PROJECTILE_VISUAL_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Node3D;
using godot::Vector3;

/// Visual representation for a projectile in flight
/// This is a placeholder that logs what's happening
/// Can be replaced with actual mesh/particle visuals later
///
/// Subclasses can override to add:
/// - Mesh instances
/// - Particle effects
/// - Trail effects
/// - Sound effects
/// - Animation
class ProjectileVisual : public Node3D {
  GDCLASS(ProjectileVisual, Node3D)

 protected:
  static void _bind_methods();

  // Override these for custom visuals
  virtual void _on_spawn(const Vector3& position, const Vector3& direction);
  virtual void _on_travel(const Vector3& position, float distance_traveled);
  virtual void _on_impact(const Vector3& position);

 public:
  ProjectileVisual();
  ~ProjectileVisual();

  void _physics_process(double delta) override;

  // Public interface
  void on_spawn(const Vector3& position, const Vector3& direction);
  void on_travel(const Vector3& position, float distance_traveled);
  void on_impact(const Vector3& position);

  // Configuration
  void set_projectile_speed(float speed);
  float get_projectile_speed() const;

  void set_aoe_radius(float radius);
  float get_aoe_radius() const;

  void set_impact_radius(float radius);
  float get_impact_radius() const;

 private:
  float projectile_speed = 30.0f;
  float aoe_radius = 5.0f;
  float impact_radius = 0.5f;
};

#endif  // GDEXTENSION_PROJECTILE_VISUAL_H
