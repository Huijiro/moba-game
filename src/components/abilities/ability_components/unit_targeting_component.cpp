#include "unit_targeting_component.hpp"

#include <godot_cpp/classes/decal.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
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
using godot::Variant;

UnitTargetingComponent::UnitTargetingComponent() = default;

UnitTargetingComponent::~UnitTargetingComponent() = default;

void UnitTargetingComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_range", "range"),
                       &UnitTargetingComponent::set_range);
  ClassDB::bind_method(D_METHOD("get_range"),
                       &UnitTargetingComponent::get_range);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "range"),
               "set_range", "get_range");

  ClassDB::bind_method(D_METHOD("_on_validate", "context"),
                       &UnitTargetingComponent::_on_validate);
  ClassDB::bind_method(D_METHOD("_on_query_targeting", "info"),
                       &UnitTargetingComponent::_on_query_targeting);
}

void UnitTargetingComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("validate",
                     godot::Callable(this, "_on_validate"));
    ability->connect("query_targeting",
                     godot::Callable(this, "_on_query_targeting"));
  }
}

void UnitTargetingComponent::set_range(float r) { range = r; }
float UnitTargetingComponent::get_range() const { return range; }

void UnitTargetingComponent::_on_validate(const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) {
    return;
  }

  Unit* caster = ctx->get_caster();
  Unit* target = ctx->get_target();

  if (target == nullptr) {
    // No unit target — this component requires one
    ctx->block("no_target");
    return;
  }

  if (range <= 0.0f) {
    return;  // Infinite range
  }

  float distance = caster->get_global_position().distance_to(
      target->get_global_position());

  if (distance > range) {
    ctx->block("out_of_range");
    DBG_DEBUG("UnitTargetingComponent",
              "Out of range: " + godot::String::num(distance, 1) + " > " +
                  godot::String::num(range, 1));
  }
}

void UnitTargetingComponent::_on_query_targeting(
    const Ref<RefCounted>& info) {
  Ref<TargetingInfo> targeting = info;
  if (targeting.is_null()) {
    return;
  }
  targeting->set_type(TargetingInfo::UNIT);
  targeting->set_range(range);
}

// --- Preview interface ---

bool UnitTargetingComponent::has_preview() const { return range > 0.0f; }

godot::Node3D* UnitTargetingComponent::create_preview() {
  auto* resource_loader = godot::ResourceLoader::get_singleton();
  Ref<godot::PackedScene> scene =
      resource_loader->load("res://previews/range_circle.tscn");
  if (scene.is_null()) return nullptr;

  auto* preview = godot::Object::cast_to<godot::Node3D>(scene->instantiate());
  if (preview == nullptr) return nullptr;

  // Decal3D size: (diameter, projection_height, diameter)
  float diameter = range * 2.0f;
  auto* decal = godot::Object::cast_to<godot::Decal>(preview);
  if (decal != nullptr) {
    decal->set_size(godot::Vector3(diameter, 10.0f, diameter));
  } else {
    preview->set_scale(godot::Vector3(range, 1.0f, range));
  }
  return preview;
}

void UnitTargetingComponent::update_preview(godot::Node3D* preview,
                                            const godot::Vector3& caster_pos,
                                            const godot::Vector3& ground_pos) {
  if (preview == nullptr) return;
  // Range circle stays centered on caster, slightly above ground
  godot::Vector3 pos = caster_pos;
  pos.y += 0.1f;
  preview->set_global_position(pos);
}
