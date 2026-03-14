#ifndef INSTANT_CAST_COMPONENT_HPP
#define INSTANT_CAST_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

/// Marks an ability as instant-cast.
/// On activated, immediately emits execute and completed.
class InstantCastComponent : public AbilitySubcomponent {
  GDCLASS(InstantCastComponent, AbilitySubcomponent);

 public:
  InstantCastComponent();
  ~InstantCastComponent();

  void _ready() override;

 protected:
  static void _bind_methods();

 private:
  void _on_activated(const godot::Ref<godot::RefCounted>& context);
};

#endif  // INSTANT_CAST_COMPONENT_HPP
