#include "aoe_damage_component.hpp"

#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_shape_query_parameters3d.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../../common/collision_layers.hpp"
#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "point_targeting_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
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

  ClassDB::bind_method(D_METHOD("set_damage_caster", "damage"),
                       &AoEDamageComponent::set_damage_caster);
  ClassDB::bind_method(D_METHOD("get_damage_caster"),
                       &AoEDamageComponent::get_damage_caster);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "damage_caster"), "set_damage_caster",
               "get_damage_caster");

  ClassDB::bind_method(D_METHOD("get_ability"),
                       &AoEDamageComponent::get_ability);

  ClassDB::bind_method(D_METHOD("_on_execute", "context"),
                       &AoEDamageComponent::_on_execute);
}

void AoEDamageComponent::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) return;

  // AoE area: ability layer, not monitorable — shape only used for queries
  set_monitoring(false);
  set_monitorable(false);
  set_collision_layer(CollisionLayer::ABILITIES);
  set_collision_mask(0);  // We query manually, don't need mask

  // Find parent AbilityNode
  owner_ability = Object::cast_to<AbilityNode>(get_parent());
  if (owner_ability == nullptr) {
    godot::UtilityFunctions::push_error(
        "[AoEDamageComponent] must be a child of AbilityNode");
    return;
  }

  owner_ability->connect("execute", godot::Callable(this, "_on_execute"));

  // Validate we have a CollisionShape3D child
  bool has_shape = false;
  for (int i = 0; i < get_child_count(); i++) {
    auto* cs = Object::cast_to<godot::CollisionShape3D>(get_child(i));
    if (cs != nullptr && cs->get_shape().is_valid()) {
      has_shape = true;
      break;
    }
  }
  if (!has_shape) {
    DBG_WARN("AoEDamage",
             "No CollisionShape3D child — add one to define the AoE area");
  }
}

AbilityNode* AoEDamageComponent::get_ability() const {
  return owner_ability;
}

void AoEDamageComponent::set_base_damage(float damage) {
  base_damage = damage;
}
float AoEDamageComponent::get_base_damage() const { return base_damage; }

void AoEDamageComponent::set_damage_caster(bool damage) {
  damage_caster = damage;
}
bool AoEDamageComponent::get_damage_caster() const { return damage_caster; }

void AoEDamageComponent::_on_execute(const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null() || base_damage <= 0.0f) return;

  Unit* caster = ctx->get_caster();
  Vector3 center = ctx->get_position();

  if (center == Vector3() && ctx->get_target() != nullptr) {
    center = ctx->get_target()->get_global_position();
  }

  if (center == Vector3()) {
    DBG_WARN("AoEDamage", "No target position for AoE");
    return;
  }

  // Find the shape from our CollisionShape3D child
  Ref<godot::Shape3D> shape;
  for (int i = 0; i < get_child_count(); i++) {
    auto* cs = Object::cast_to<godot::CollisionShape3D>(get_child(i));
    if (cs != nullptr && cs->get_shape().is_valid()) {
      shape = cs->get_shape();
      break;
    }
  }

  if (shape.is_null()) return;

  // Physics space query
  Ref<godot::World3D> world = get_world_3d();
  if (world.is_null()) return;

  godot::PhysicsDirectSpaceState3D* space = world->get_direct_space_state();
  if (space == nullptr) return;

  Ref<godot::PhysicsShapeQueryParameters3D> query;
  query.instantiate();
  query->set_shape(shape);
  godot::Transform3D xform;
  xform.origin = center;
  query->set_transform(xform);
  query->set_collide_with_bodies(true);
  query->set_collide_with_areas(false);
  query->set_collision_mask(CollisionLayer::UNITS);

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

// --- Preview interface ---

float AoEDamageComponent::_get_aoe_radius() const {
  for (int i = 0; i < get_child_count(); i++) {
    auto* cs = Object::cast_to<godot::CollisionShape3D>(get_child(i));
    if (cs == nullptr || cs->get_shape().is_null()) continue;
    auto sphere = godot::Ref<godot::SphereShape3D>(cs->get_shape());
    if (sphere.is_valid()) return sphere->get_radius();
  }
  return 0.0f;
}

bool AoEDamageComponent::has_preview() const {
  return _get_aoe_radius() > 0.0f;
}

godot::Node3D* AoEDamageComponent::create_preview() {
  float radius = _get_aoe_radius();
  if (radius <= 0.0f) return nullptr;

  auto* resource_loader = godot::ResourceLoader::get_singleton();
  Ref<godot::PackedScene> scene =
      resource_loader->load("res://previews/aoe_circle.tscn");
  if (scene.is_null()) return nullptr;

  auto* preview = Object::cast_to<godot::Node3D>(scene->instantiate());
  if (preview == nullptr) return nullptr;

  preview->set_scale(Vector3(radius, 1.0f, radius));
  return preview;
}

void AoEDamageComponent::update_preview(godot::Node3D* preview,
                                        const Vector3& caster_pos,
                                        const Vector3& ground_pos) {
  if (preview == nullptr) return;

  // AoE circle follows cursor, clamped to range
  // Find sibling PointTargetingComponent range for clamping
  float max_range = 0.0f;
  if (owner_ability != nullptr) {
    for (int i = 0; i < owner_ability->get_child_count(); i++) {
      auto* pt = Object::cast_to<PointTargetingComponent>(
          owner_ability->get_child(i));
      if (pt != nullptr) {
        max_range = pt->get_max_range();
        break;
      }
    }
  }

  Vector3 target = ground_pos;
  if (max_range > 0.0f) {
    Vector3 offset = ground_pos - caster_pos;
    offset.y = 0.0f;
    if (offset.length() > max_range) {
      offset = offset.normalized() * max_range;
      target = caster_pos + offset;
    }
  }

  target.y = 0.05f;
  preview->set_global_position(target);
}

void AoEDamageComponent::cleanup_preview(godot::Node3D* preview) {
  if (preview != nullptr) preview->queue_free();
}
