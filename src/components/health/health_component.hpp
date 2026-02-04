#ifndef GDEXTENSION_HEALTH_COMPONENT_H
#define GDEXTENSION_HEALTH_COMPONENT_H

#include "../unit_component.hpp"

class HealthComponent : public UnitComponent {
  GDCLASS(HealthComponent, UnitComponent)

 protected:
  static void _bind_methods();

  float max_health = 100.0f;
  float current_health = 100.0f;
  bool is_dead_flag = false;

 public:
  HealthComponent();
  ~HealthComponent();

  void set_max_health(float value);
  float get_max_health() const;

  void set_current_health(float value);
  float get_current_health() const;

  // Returns true if unit died from this damage
  bool apply_damage(float amount, godot::Object* source = nullptr);
  void heal(float amount);
  bool is_dead() const;

  // Debug label registration
  void register_debug_labels(LabelRegistry* registry) override;

 private:
  // Disable collision shapes when unit dies
  void _disable_collision();
};

#endif  // GDEXTENSION_HEALTH_COMPONENT_H
