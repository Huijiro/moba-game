#include "ability_range_module.hpp"

#include <godot_cpp/core/class_db.hpp>

using godot::ClassDB;
using godot::D_METHOD;

AbilityRangeModule::AbilityRangeModule() = default;

AbilityRangeModule::~AbilityRangeModule() = default;

void AbilityRangeModule::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_range"), &AbilityRangeModule::get_range);
}

float AbilityRangeModule::get_range() const {
  return 0.0f;
}
