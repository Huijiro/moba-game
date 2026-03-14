#ifndef AOE_DAMAGE_COMPONENT_HPP
#define AOE_DAMAGE_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

namespace godot {
class CollisionShape3D;
}

class Unit;

/// Deals damage to all units overlapping a shape at the target position.
/// Uses Godot's physics space query (intersect_shape) for instant area detection.
///
/// Add a CollisionShape3D child in the editor to define the AoE area.
/// The shape gizmo renders in the editor so you can see and tweak the area.
/// Supports SphereShape3D, BoxShape3D, CylinderShape3D, etc.
///
/// Listens to execute. Uses context position (or target unit position) as center.
class AoEDamageComponent : public AbilitySubcomponent {
  GDCLASS(AoEDamageComponent, AbilitySubcomponent);

 public:
  AoEDamageComponent();
  ~AoEDamageComponent();

  void _ready() override;

  void set_base_damage(float damage);
  float get_base_damage() const;

  void set_damage_caster(bool damage);
  bool get_damage_caster() const;

 protected:
  static void _bind_methods();

 private:
  float base_damage = 0.0f;
  bool damage_caster = false;

  godot::CollisionShape3D* shape_node = nullptr;

  void _on_execute(const godot::Ref<godot::RefCounted>& context);

  /// Find first CollisionShape3D child
  godot::CollisionShape3D* _find_shape_child() const;
};

#endif  // AOE_DAMAGE_COMPONENT_HPP
