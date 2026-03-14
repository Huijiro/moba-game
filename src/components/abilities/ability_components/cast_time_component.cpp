#include "cast_time_component.hpp"

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

CastTimeComponent::CastTimeComponent() = default;

CastTimeComponent::~CastTimeComponent() = default;

void CastTimeComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_cast_time", "time"),
                       &CastTimeComponent::set_cast_time);
  ClassDB::bind_method(D_METHOD("get_cast_time"),
                       &CastTimeComponent::get_cast_time);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cast_time"),
               "set_cast_time", "get_cast_time");

  ClassDB::bind_method(D_METHOD("set_cast_point", "point"),
                       &CastTimeComponent::set_cast_point);
  ClassDB::bind_method(D_METHOD("get_cast_point"),
                       &CastTimeComponent::get_cast_point);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cast_point"),
               "set_cast_point", "get_cast_point");

  ClassDB::bind_method(D_METHOD("_on_activated", "context"),
                       &CastTimeComponent::_on_activated);
}

void CastTimeComponent::_ready() {
  AbilitySubcomponent::_ready();

  set_physics_process(false);  // Only process when actively casting

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("activated",
                     godot::Callable(this, "_on_activated"));
  }
}

void CastTimeComponent::_physics_process(double delta) {
  if (!casting) {
    return;
  }

  elapsed += delta;

  // Check cast point
  float cast_point_time = cast_time * cast_point;
  if (!point_reached && elapsed >= cast_point_time) {
    point_reached = true;
    AbilityNode* ability = get_ability();
    if (ability != nullptr) {
      DBG_INFO("CastTimeComponent", "Cast point reached at " +
                   godot::String::num(elapsed, 2) + "s");
      ability->emit_signal("execute", current_context);
    }
  }

  // Check cast finished
  if (elapsed >= cast_time) {
    casting = false;
    set_physics_process(false);
    AbilityNode* ability = get_ability();
    if (ability != nullptr) {
      DBG_INFO("CastTimeComponent", "Cast finished");
      ability->emit_signal("completed", current_context);
    }
    current_context = Ref<RefCounted>();
  }
}

void CastTimeComponent::set_cast_time(float t) { cast_time = t; }
float CastTimeComponent::get_cast_time() const { return cast_time; }

void CastTimeComponent::set_cast_point(float p) { cast_point = p; }
float CastTimeComponent::get_cast_point() const { return cast_point; }

void CastTimeComponent::_on_activated(const Ref<RefCounted>& context) {
  elapsed = 0.0f;
  casting = true;
  point_reached = false;
  current_context = context;
  set_physics_process(true);
  DBG_INFO("CastTimeComponent", "Cast started, duration=" +
               godot::String::num(cast_time, 2) + "s");
}
