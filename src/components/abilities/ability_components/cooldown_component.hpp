#ifndef COOLDOWN_COMPONENT_HPP
#define COOLDOWN_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

/// Manages ability cooldown.
/// On validate: blocks if on cooldown.
/// On completed: starts cooldown timer, emits cooldown_started/finished.
class CooldownComponent : public AbilitySubcomponent {
  GDCLASS(CooldownComponent, AbilitySubcomponent);

 public:
  CooldownComponent();
  ~CooldownComponent();

  void _ready() override;
  void _physics_process(double delta) override;

  void set_cooldown(float duration);
  float get_cooldown() const;

  bool is_on_cooldown() const;
  float get_remaining() const;

 protected:
  static void _bind_methods();

 private:
  float cooldown = 1.0f;
  float remaining = 0.0f;

  void _on_validate(const godot::Ref<godot::RefCounted>& context);
  void _on_completed(const godot::Ref<godot::RefCounted>& context);
};

#endif  // COOLDOWN_COMPONENT_HPP
