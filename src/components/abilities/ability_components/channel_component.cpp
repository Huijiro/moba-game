#include "channel_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::Variant;

ChannelComponent::ChannelComponent() = default;

ChannelComponent::~ChannelComponent() = default;

void ChannelComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_channel_duration", "duration"),
                       &ChannelComponent::set_channel_duration);
  ClassDB::bind_method(D_METHOD("get_channel_duration"),
                       &ChannelComponent::get_channel_duration);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "channel_duration"),
               "set_channel_duration", "get_channel_duration");

  ClassDB::bind_method(D_METHOD("set_tick_interval", "interval"),
                       &ChannelComponent::set_tick_interval);
  ClassDB::bind_method(D_METHOD("get_tick_interval"),
                       &ChannelComponent::get_tick_interval);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tick_interval"),
               "set_tick_interval", "get_tick_interval");

  ClassDB::bind_method(D_METHOD("_on_activated", "context"),
                       &ChannelComponent::_on_activated);
}

void ChannelComponent::_ready() {
  AbilitySubcomponent::_ready();

  set_physics_process(false);

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("activated",
                     godot::Callable(this, "_on_activated"));
  }
}

void ChannelComponent::_physics_process(double delta) {
  if (!channeling) {
    return;
  }

  elapsed += delta;

  // Fire first tick immediately as execute
  if (!first_tick_fired) {
    first_tick_fired = true;
    AbilityNode* ability = get_ability();
    if (ability != nullptr) {
      ability->emit_signal("execute", current_context);
    }
    next_tick_time = tick_interval;
  }

  // Periodic ticks
  if (tick_interval > 0.0f && elapsed >= next_tick_time) {
    AbilityNode* ability = get_ability();
    if (ability != nullptr) {
      ability->emit_signal("tick", current_context);
    }
    next_tick_time += tick_interval;
  }

  // Channel finished
  if (elapsed >= channel_duration) {
    channeling = false;
    set_physics_process(false);
    AbilityNode* ability = get_ability();
    if (ability != nullptr) {
      DBG_INFO("ChannelComponent", "Channel finished");
      ability->emit_signal("completed", current_context);
    }
    current_context = Ref<RefCounted>();
  }
}

void ChannelComponent::set_channel_duration(float duration) {
  channel_duration = duration;
}

float ChannelComponent::get_channel_duration() const {
  return channel_duration;
}

void ChannelComponent::set_tick_interval(float interval) {
  tick_interval = interval;
}

float ChannelComponent::get_tick_interval() const {
  return tick_interval;
}

void ChannelComponent::_on_activated(const Ref<RefCounted>& context) {
  elapsed = 0.0f;
  next_tick_time = 0.0f;
  channeling = true;
  first_tick_fired = false;
  current_context = context;
  set_physics_process(true);
  DBG_INFO("ChannelComponent", "Channel started, duration=" +
               godot::String::num(channel_duration, 2) + "s");
}
