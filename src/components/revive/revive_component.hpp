#ifndef GDEXTENSION_REVIVE_COMPONENT_H
#define GDEXTENSION_REVIVE_COMPONENT_H

#include "../unit_component.hpp"

class HealthComponent;

class ReviveComponent : public UnitComponent {
  GDCLASS(ReviveComponent, UnitComponent)

 protected:
  static void _bind_methods();

  float revive_time = 5.0f;  // Time in seconds before unit revives
  double revive_timer = 0.0;
  bool is_reviving = false;

  // Signal handler for death signal
  void _on_unit_died(godot::Object* source);

 public:
  ReviveComponent();
  ~ReviveComponent();

  void _ready() override;
  void _physics_process(double delta) override;

  void set_revive_time(float time);
  float get_revive_time() const;

  // Start the revive process
  void start_revive();

  // Debug label registration
  void register_debug_labels(LabelRegistry* registry) override;

 private:
  HealthComponent* health_component = nullptr;

  // Re-enable collision shapes when unit revives
  void _enable_collision();
};

#endif  // GDEXTENSION_REVIVE_COMPONENT_H
