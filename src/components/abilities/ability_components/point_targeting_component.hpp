#ifndef POINT_TARGETING_COMPONENT_HPP
#define POINT_TARGETING_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

/// Marks an ability as point-targeted with a max cast range.
/// On validate: blocks if no position provided or position out of range.
class PointTargetingComponent : public AbilitySubcomponent {
  GDCLASS(PointTargetingComponent, AbilitySubcomponent);

 public:
  PointTargetingComponent();
  ~PointTargetingComponent();

  void _ready() override;

  void set_max_range(float r);
  float get_max_range() const;

  // Preview interface
  bool has_preview() const override;
  godot::Node3D* create_preview() override;
  void update_preview(godot::Node3D* preview, const godot::Vector3& caster_pos,
                      const godot::Vector3& ground_pos) override;

 protected:
  static void _bind_methods();

 private:
  float max_range = 20.0f;

  void _on_validate(const godot::Ref<godot::RefCounted>& context);
  void _on_query_targeting(const godot::Ref<godot::RefCounted>& info);
};

#endif  // POINT_TARGETING_COMPONENT_HPP
