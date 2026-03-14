#include "point_targeting_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../targeting_info.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::String;
using godot::Variant;
using godot::Vector3;

PointTargetingComponent::PointTargetingComponent() = default;

PointTargetingComponent::~PointTargetingComponent() = default;

void PointTargetingComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_max_range", "range"),
                       &PointTargetingComponent::set_max_range);
  ClassDB::bind_method(D_METHOD("get_max_range"),
                       &PointTargetingComponent::get_max_range);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_range"),
               "set_max_range", "get_max_range");

  ClassDB::bind_method(D_METHOD("_on_validate", "context"),
                       &PointTargetingComponent::_on_validate);
  ClassDB::bind_method(D_METHOD("_on_query_targeting", "info"),
                       &PointTargetingComponent::_on_query_targeting);
}

void PointTargetingComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("validate",
                     godot::Callable(this, "_on_validate"));
    ability->connect("query_targeting",
                     godot::Callable(this, "_on_query_targeting"));
  }
}

void PointTargetingComponent::set_max_range(float r) { max_range = r; }
float PointTargetingComponent::get_max_range() const { return max_range; }

void PointTargetingComponent::_on_validate(
    const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) {
    return;
  }

  Vector3 position = ctx->get_position();
  if (position == Vector3()) {
    ctx->block("no_target_position");
    return;
  }

  if (max_range <= 0.0f) {
    return;  // Infinite range
  }

  Unit* caster = ctx->get_caster();
  if (caster == nullptr) {
    return;
  }

  float distance = caster->get_global_position().distance_to(position);
  if (distance > max_range) {
    ctx->block("out_of_range");
    DBG_DEBUG("PointTargetingComponent",
              "Out of range: " + String::num(distance, 1) + " > " +
                  String::num(max_range, 1));
  }
}

void PointTargetingComponent::_on_query_targeting(
    const Ref<RefCounted>& info) {
  Ref<TargetingInfo> targeting = info;
  if (targeting.is_null()) {
    return;
  }
  targeting->set_type(TargetingInfo::POINT);
  targeting->set_range(max_range);
}
