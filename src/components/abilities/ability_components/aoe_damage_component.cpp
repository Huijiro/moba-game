#include "aoe_damage_component.hpp"

#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_shape_query_parameters3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../ability_context.hpp"
#include "../ability_node.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::String;
using godot::Variant;
using godot::Vector3;

AoEDamageComponent::AoEDamageComponent() = default;

AoEDamageComponent::~AoEDamageComponent() = default;

void AoEDamageComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_base_damage", "damage"),
                       &AoEDamageComponent::set_base_damage);
  ClassDB::bind_method(D_METHOD("get_base_damage"),
                       &AoEDamageComponent::get_base_damage);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_damage"), "set_base_damage",
               "get_base_damage");

  ClassDB::bind_method(D_METHOD("set_shape", "shape"),
                       &AoEDamageComponent::set_shape);
  ClassDB::bind_method(D_METHOD("get_shape"),
                       &AoEDamageComponent::get_shape);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shape",
                            godot::PROPERTY_HINT_RESOURCE_TYPE, "Shape3D"),
               "set_shape", "get_shape");

  ClassDB::bind_method(D_METHOD("set_damage_caster", "damage"),
                       &AoEDamageComponent::set_damage_caster);
  ClassDB::bind_method(D_METHOD("get_damage_caster"),
                       &AoEDamageComponent::get_damage_caster);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "damage_caster"), "set_damage_caster",
               "get_damage_caster");

  ClassDB::bind_method(D_METHOD("_on_execute", "context"),
                       &AoEDamageComponent::_on_execute);
}

void AoEDamageComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("execute", godot::Callable(this, "_on_execute"));
  }

  if (shape.is_null()) {
    DBG_WARN("AoEDamage", "No shape configured — AoE won't detect anything");
  }
}

void AoEDamageComponent::set_base_damage(float damage) {
  base_damage = damage;
}
float AoEDamageComponent::get_base_damage() const { return base_damage; }

void AoEDamageComponent::set_shape(const Ref<godot::Shape3D>& s) { shape = s; }
Ref<godot::Shape3D> AoEDamageComponent::get_shape() const { return shape; }

void AoEDamageComponent::set_damage_caster(bool damage) {
  damage_caster = damage;
}
bool AoEDamageComponent::get_damage_caster() const { return damage_caster; }

void AoEDamageComponent::_on_execute(const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) return;

  if (shape.is_null() || base_damage <= 0.0f) return;

  Unit* caster = ctx->get_caster();
  Vector3 center = ctx->get_position();

  // Fall back to target unit's position
  if (center == Vector3() && ctx->get_target() != nullptr) {
    center = ctx->get_target()->get_global_position();
  }

  if (center == Vector3()) {
    DBG_WARN("AoEDamage", "No target position for AoE");
    return;
  }

  // Get physics space from the ability node
  godot::Node3D* spatial = Object::cast_to<godot::Node3D>(get_ability());
  if (spatial == nullptr || !spatial->is_inside_tree()) return;

  Ref<godot::World3D> world = spatial->get_world_3d();
  if (world.is_null()) return;

  godot::PhysicsDirectSpaceState3D* space = world->get_direct_space_state();
  if (space == nullptr) return;

  // Setup query with the configured shape
  Ref<godot::PhysicsShapeQueryParameters3D> query;
  query.instantiate();
  query->set_shape(shape);
  godot::Transform3D xform;
  xform.origin = center;
  query->set_transform(xform);
  query->set_collide_with_bodies(true);
  query->set_collide_with_areas(false);

  // Execute the query
  godot::TypedArray<godot::Dictionary> results = space->intersect_shape(query);

  int hit_count = 0;
  for (int i = 0; i < results.size(); i++) {
    godot::Dictionary result = results[i];
    Object* collider = Object::cast_to<Object>(result["collider"]);
    Unit* unit = Object::cast_to<Unit>(collider);

    if (unit == nullptr || !unit->is_inside_tree()) continue;
    if (!damage_caster && unit == caster) continue;

    unit->relay(get_take_damage(), base_damage,
                caster != nullptr ? (godot::Object*)caster : nullptr);
    hit_count++;
  }

  AbilityNode* ability = get_ability();
  String ability_name = ability ? ability->get_ability_name() : "Unknown";
  DBG_INFO("AoEDamage",
           ability_name + " hit " + String::num(hit_count) +
               " units for " + String::num(base_damage, 1) + " damage");
}
