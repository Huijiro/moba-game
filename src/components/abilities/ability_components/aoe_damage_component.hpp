#ifndef AOE_DAMAGE_COMPONENT_HPP
#define AOE_DAMAGE_COMPONENT_HPP

#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

class AbilityNode;
class Unit;

/// Deals damage to all units overlapping this Area3D at the target position.
/// Uses Godot's physics space query (intersect_shape) for instant detection.
///
/// Extends Area3D directly — add a CollisionShape3D child to define the area.
/// The shape gizmo renders in the editor. Monitoring is disabled; the shape
/// is only used for one-shot physics queries on execute.
///
/// Listens to execute. Uses context position (or target unit position) as center.
class AoEDamageComponent : public godot::Area3D {
  GDCLASS(AoEDamageComponent, godot::Area3D);

 public:
  AoEDamageComponent();
  ~AoEDamageComponent();

  void _ready() override;

  void set_base_damage(float damage);
  float get_base_damage() const;

  void set_damage_caster(bool damage);
  bool get_damage_caster() const;

  AbilityNode* get_ability() const;

  // Preview interface (mirrors AbilitySubcomponent)
  bool has_preview() const;
  godot::Node3D* create_preview();
  void update_preview(godot::Node3D* preview, const godot::Vector3& caster_pos,
                      const godot::Vector3& ground_pos);
  void cleanup_preview(godot::Node3D* preview);

 protected:
  static void _bind_methods();

 private:
  float _get_aoe_radius() const;
  float base_damage = 0.0f;
  bool damage_caster = false;
  AbilityNode* owner_ability = nullptr;

  void _on_execute(const godot::Ref<godot::RefCounted>& context);
};

#endif  // AOE_DAMAGE_COMPONENT_HPP
