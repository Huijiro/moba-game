#ifndef CHANNEL_COMPONENT_HPP
#define CHANNEL_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

/// Manages channel-type abilities with periodic ticks.
/// On activated: begins timer, emits tick periodically,
/// emits execute on first tick, emits completed when done.
class ChannelComponent : public AbilitySubcomponent {
  GDCLASS(ChannelComponent, AbilitySubcomponent);

 public:
  ChannelComponent();
  ~ChannelComponent();

  void _ready() override;
  void _physics_process(double delta) override;

  void set_channel_duration(float duration);
  float get_channel_duration() const;

  void set_tick_interval(float interval);
  float get_tick_interval() const;

 protected:
  static void _bind_methods();

 private:
  float channel_duration = 2.0f;
  float tick_interval = 0.5f;

  // Runtime state
  float elapsed = 0.0f;
  float next_tick_time = 0.0f;
  bool channeling = false;
  bool first_tick_fired = false;
  godot::Ref<godot::RefCounted> current_context;

  void _on_activated(const godot::Ref<godot::RefCounted>& context);
};

#endif  // CHANNEL_COMPONENT_HPP
