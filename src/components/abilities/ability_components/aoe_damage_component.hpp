#ifndef AOE_DAMAGE_COMPONENT_HPP
#define AOE_DAMAGE_COMPONENT_HPP

#include <godot_cpp/classes/shape3d.hpp>

#include "../ability_subcomponent.hpp"

class Unit;

/// Deals damage to all units overlapping a shape at the target position.
/// Uses Godot's physics space query (intersect_shape) for instant area detection.
///
/// The collision shape is an editor-configurable Shape3D resource (SphereShape3D,
/// BoxShape3D, CylinderShape3D, etc.). Set it in the inspector.
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

  void set_shape(const godot::Ref<godot::Shape3D>& s);
  godot::Ref<godot::Shape3D> get_shape() const;

  void set_damage_caster(bool damage);
  bool get_damage_caster() const;

 protected:
  static void _bind_methods();

 private:
  float base_damage = 0.0f;
  godot::Ref<godot::Shape3D> shape;
  bool damage_caster = false;

  void _on_execute(const godot::Ref<godot::RefCounted>& context);
};

#endif  // AOE_DAMAGE_COMPONENT_HPP
