/// @file animation_binding.cpp
/// Animation binding implementation

#include "animation_binding.hpp"

#include <godot_cpp/core/class_db.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::StringName;
using godot::Variant;

AnimationBinding::AnimationBinding() = default;

AnimationBinding::~AnimationBinding() = default;

void AnimationBinding::_bind_methods() {
  // Animation name property
  ClassDB::bind_method(D_METHOD("set_animation_name", "name"),
                       &AnimationBinding::set_animation_name);
  ClassDB::bind_method(D_METHOD("get_animation_name"),
                       &AnimationBinding::get_animation_name);
  ClassDB::add_property("AnimationBinding",
                        PropertyInfo(Variant::STRING_NAME, "animation_name"),
                        "set_animation_name", "get_animation_name");

  // Trigger signal property
  ClassDB::bind_method(D_METHOD("set_trigger_signal", "signal"),
                       &AnimationBinding::set_trigger_signal);
  ClassDB::bind_method(D_METHOD("get_trigger_signal"),
                       &AnimationBinding::get_trigger_signal);
  ClassDB::add_property("AnimationBinding",
                        PropertyInfo(Variant::STRING_NAME, "trigger_signal"),
                        "set_trigger_signal", "get_trigger_signal");

  // Playback speed property
  ClassDB::bind_method(D_METHOD("set_playback_speed", "speed"),
                       &AnimationBinding::set_playback_speed);
  ClassDB::bind_method(D_METHOD("get_playback_speed"),
                       &AnimationBinding::get_playback_speed);
  ClassDB::add_property("AnimationBinding",
                        PropertyInfo(Variant::FLOAT, "playback_speed",
                                     godot::PROPERTY_HINT_RANGE, "0.1,2.0,0.1"),
                        "set_playback_speed", "get_playback_speed");
}

void AnimationBinding::set_animation_name(const StringName& name) {
  animation_name = name;
}

StringName AnimationBinding::get_animation_name() const {
  return animation_name;
}

void AnimationBinding::set_trigger_signal(const StringName& signal) {
  trigger_signal = signal;
}

StringName AnimationBinding::get_trigger_signal() const {
  return trigger_signal;
}

void AnimationBinding::set_playback_speed(float speed) {
  playback_speed = godot::Math::max(0.1f, speed);
}

float AnimationBinding::get_playback_speed() const {
  return playback_speed;
}
