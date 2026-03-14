#ifndef UNIT_TARGETING_COMPONENT_HPP
#define UNIT_TARGETING_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

/// Marks an ability as unit-targeted with a range.
/// On validate: blocks if target is out of range.
class UnitTargetingComponent : public AbilitySubcomponent {
  GDCLASS(UnitTargetingComponent, AbilitySubcomponent);

 public:
  UnitTargetingComponent();
  ~UnitTargetingComponent();

  void _ready() override;

  void set_range(float r);
  float get_range() const;

  // Preview interface
  bool has_preview() const override;
  godot::Node3D* create_preview() override;
  void update_preview(godot::Node3D* preview, const godot::Vector3& caster_pos,
                      const godot::Vector3& ground_pos) override;

 protected:
  static void _bind_methods();

 private:
  float range = 5.0f;

  void _on_validate(const godot::Ref<godot::RefCounted>& context);
  void _on_query_targeting(const godot::Ref<godot::RefCounted>& info);
};

#endif  // UNIT_TARGETING_COMPONENT_HPP
