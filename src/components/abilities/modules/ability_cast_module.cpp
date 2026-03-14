#include "ability_cast_module.hpp"

#include <godot_cpp/core/class_db.hpp>

using godot::ClassDB;
using godot::D_METHOD;

AbilityCastModule::AbilityCastModule() = default;

AbilityCastModule::~AbilityCastModule() = default;

void AbilityCastModule::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_cast_type"),
                       &AbilityCastModule::get_cast_type);
  ClassDB::bind_method(D_METHOD("get_cast_duration"),
                       &AbilityCastModule::get_cast_duration);
  ClassDB::bind_method(D_METHOD("get_cast_point"),
                       &AbilityCastModule::get_cast_point);
  ClassDB::bind_method(D_METHOD("get_channel_duration"),
                       &AbilityCastModule::get_channel_duration);
  ClassDB::bind_method(D_METHOD("get_tick_interval"),
                       &AbilityCastModule::get_tick_interval);
}

int AbilityCastModule::get_cast_type() const {
  return -1;
}

float AbilityCastModule::get_cast_duration() const {
  return 0.0f;
}

float AbilityCastModule::get_cast_point() const {
  return 0.0f;
}

float AbilityCastModule::get_channel_duration() const {
  return 0.0f;
}

float AbilityCastModule::get_tick_interval() const {
  return 0.0f;
}
