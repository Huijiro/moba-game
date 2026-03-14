#ifndef RESOURCE_COST_COMPONENT_HPP
#define RESOURCE_COST_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

class ResourcePoolComponent;

/// Manages ability resource cost.
/// On validate: blocks if unit can't afford.
/// On activated: deducts cost from resource pool.
class ResourceCostComponent : public AbilitySubcomponent {
  GDCLASS(ResourceCostComponent, AbilitySubcomponent);

 public:
  ResourceCostComponent();
  ~ResourceCostComponent();

  void _ready() override;

  void set_resource_pool_id(const godot::String& pool_id);
  godot::String get_resource_pool_id() const;

  void set_resource_cost(float cost);
  float get_resource_cost() const;

 protected:
  static void _bind_methods();

 private:
  godot::String resource_pool_id = "default";
  float resource_cost = 0.0f;

  ResourcePoolComponent* _find_pool(const godot::Ref<godot::RefCounted>& context);

  void _on_validate(const godot::Ref<godot::RefCounted>& context);
  void _on_activated(const godot::Ref<godot::RefCounted>& context);
};

#endif  // RESOURCE_COST_COMPONENT_HPP
