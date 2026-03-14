#ifndef CAST_TIME_COMPONENT_HPP
#define CAST_TIME_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

/// Manages cast time for abilities with a wind-up period.
/// On activated: begins timer, emits execute at cast_point,
/// emits completed when cast_time expires.
class CastTimeComponent : public AbilitySubcomponent {
  GDCLASS(CastTimeComponent, AbilitySubcomponent);

 public:
  CastTimeComponent();
  ~CastTimeComponent();

  void _ready() override;
  void _physics_process(double delta) override;

  void set_cast_time(float t);
  float get_cast_time() const;

  void set_cast_point(float p);
  float get_cast_point() const;

 protected:
  static void _bind_methods();

 private:
  float cast_time = 0.7f;
  float cast_point = 0.5f;  // 0.0-1.0, when in cast_time the ability fires

  // Runtime state
  float elapsed = 0.0f;
  bool casting = false;
  bool point_reached = false;
  godot::Ref<godot::RefCounted> current_context;

  void _on_activated(const godot::Ref<godot::RefCounted>& context);
};

#endif  // CAST_TIME_COMPONENT_HPP
