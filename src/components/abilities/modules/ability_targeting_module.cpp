#include "ability_targeting_module.hpp"

#include <godot_cpp/core/class_db.hpp>

using godot::ClassDB;
using godot::D_METHOD;

AbilityTargetingModule::AbilityTargetingModule() = default;

AbilityTargetingModule::~AbilityTargetingModule() = default;

void AbilityTargetingModule::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_targeting_type"),
                       &AbilityTargetingModule::get_targeting_type);
  ClassDB::bind_method(D_METHOD("requires_unit_target"),
                       &AbilityTargetingModule::requires_unit_target);
  ClassDB::bind_method(D_METHOD("requires_point_target"),
                       &AbilityTargetingModule::requires_point_target);
}

int AbilityTargetingModule::get_targeting_type() const {
  return -1;
}

bool AbilityTargetingModule::requires_unit_target() const {
  return false;
}

bool AbilityTargetingModule::requires_point_target() const {
  return false;
}
