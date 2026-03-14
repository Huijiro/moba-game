#include "targeting_info.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::Variant;

void TargetingInfo::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_type", "type"),
                       &TargetingInfo::set_type);
  ClassDB::bind_method(D_METHOD("get_type"), &TargetingInfo::get_type);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "type"), "set_type", "get_type");

  ClassDB::bind_method(D_METHOD("set_range", "range"),
                       &TargetingInfo::set_range);
  ClassDB::bind_method(D_METHOD("get_range"), &TargetingInfo::get_range);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "range"), "set_range", "get_range");

  ClassDB::bind_method(D_METHOD("set_aoe_radius", "radius"),
                       &TargetingInfo::set_aoe_radius);
  ClassDB::bind_method(D_METHOD("get_aoe_radius"),
                       &TargetingInfo::get_aoe_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "aoe_radius"), "set_aoe_radius",
               "get_aoe_radius");

  BIND_ENUM_CONSTANT(NONE);
  BIND_ENUM_CONSTANT(UNIT);
  BIND_ENUM_CONSTANT(POINT);
  BIND_ENUM_CONSTANT(SELF);
}

void TargetingInfo::set_type(int t) { targeting_type = t; }
int TargetingInfo::get_type() const { return targeting_type; }

void TargetingInfo::set_range(float r) { range = r; }
float TargetingInfo::get_range() const { return range; }

void TargetingInfo::set_aoe_radius(float r) { aoe_radius = r; }
float TargetingInfo::get_aoe_radius() const { return aoe_radius; }
