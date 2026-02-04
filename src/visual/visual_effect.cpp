#include "visual_effect.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

VisualEffect::VisualEffect() = default;

VisualEffect::~VisualEffect() = default;

void VisualEffect::_bind_methods() {
  ClassDB::bind_method(D_METHOD("play_at_position", "position"),
                       &VisualEffect::play_at_position);
  ClassDB::bind_method(
      D_METHOD("play_toward_direction", "from", "direction", "distance"),
      &VisualEffect::play_toward_direction);
  ClassDB::bind_method(D_METHOD("stop"), &VisualEffect::stop);

  ClassDB::bind_method(D_METHOD("set_duration", "seconds"),
                       &VisualEffect::set_duration);
  ClassDB::bind_method(D_METHOD("get_duration"), &VisualEffect::get_duration);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration",
               "get_duration");

  ClassDB::bind_method(D_METHOD("is_playing"), &VisualEffect::is_playing);
}

void VisualEffect::_play_at_position(const Vector3& position) {
  // Override in subclasses
  UtilityFunctions::print("[VisualEffect] Playing at (" +
                          godot::String::num(position.x) + ", " +
                          godot::String::num(position.z) + ")");
}

void VisualEffect::_play_toward_direction(const Vector3& from,
                                          const Vector3& direction,
                                          float distance) {
  // Override in subclasses
  UtilityFunctions::print(
      "[VisualEffect] Playing projectile from (" + godot::String::num(from.x) +
      ", " + godot::String::num(from.z) + ") direction (" +
      godot::String::num(direction.x) + ", " + godot::String::num(direction.z) +
      ") distance " + godot::String::num(distance));
}

void VisualEffect::_stop() {
  // Override in subclasses
  UtilityFunctions::print("[VisualEffect] Stopping");
}

void VisualEffect::play_at_position(const Vector3& position) {
  playing = true;
  _play_at_position(position);
}

void VisualEffect::play_toward_direction(const Vector3& from,
                                         const Vector3& direction,
                                         float distance) {
  playing = true;
  _play_toward_direction(from, direction, distance);
}

void VisualEffect::stop() {
  playing = false;
  _stop();
}

void VisualEffect::set_duration(float seconds) {
  duration = seconds >= 0.0f ? seconds : 1.0f;
}

float VisualEffect::get_duration() const {
  return duration;
}

bool VisualEffect::is_playing() const {
  return playing;
}
