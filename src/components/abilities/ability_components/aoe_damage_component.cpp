#include "aoe_damage_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"

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
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_damage"),
               "set_base_damage", "get_base_damage");

  ClassDB::bind_method(D_METHOD("set_radius", "radius"),
                       &AoEDamageComponent::set_radius);
  ClassDB::bind_method(D_METHOD("get_radius"),
                       &AoEDamageComponent::get_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"),
               "set_radius", "get_radius");

  ClassDB::bind_method(D_METHOD("set_damage_caster", "damage"),
                       &AoEDamageComponent::set_damage_caster);
  ClassDB::bind_method(D_METHOD("get_damage_caster"),
                       &AoEDamageComponent::get_damage_caster);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "damage_caster"),
               "set_damage_caster", "get_damage_caster");

  ClassDB::bind_method(D_METHOD("_on_execute", "context"),
                       &AoEDamageComponent::_on_execute);
}

void AoEDamageComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("execute",
                     godot::Callable(this, "_on_execute"));
  }
}

void AoEDamageComponent::set_base_damage(float damage) {
  base_damage = damage;
}

float AoEDamageComponent::get_base_damage() const { return base_damage; }

void AoEDamageComponent::set_radius(float r) { radius = r; }
float AoEDamageComponent::get_radius() const { return radius; }

void AoEDamageComponent::set_damage_caster(bool damage) {
  damage_caster = damage;
}

bool AoEDamageComponent::get_damage_caster() const { return damage_caster; }

void AoEDamageComponent::_on_execute(
    const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) {
    return;
  }

  Unit* caster = ctx->get_caster();
  Vector3 center = ctx->get_position();

  // If no position, try target unit's position
  if (center == Vector3() && ctx->get_target() != nullptr) {
    center = ctx->get_target()->get_global_position();
  }

  if (center == Vector3()) {
    DBG_WARN("AoEDamageComponent", "No target position for AoE");
    return;
  }

  _find_and_damage_units(caster, center);
}

void AoEDamageComponent::_find_and_damage_units(Unit* caster,
                                                const Vector3& center) {
  if (base_damage <= 0.0f || radius <= 0.0f) {
    return;
  }

  // Walk up to scene root to search for all units
  godot::Node* root = this;
  while (root->get_parent() != nullptr) {
    root = root->get_parent();
  }

  // Search for all Unit nodes in the scene tree
  float radius_sq = radius * radius;
  int hit_count = 0;

  // Use Godot's built-in group or tree traversal
  godot::TypedArray<godot::Node> all_nodes;
  _collect_units(root, all_nodes);

  for (int i = 0; i < all_nodes.size(); i++) {
    Unit* unit = Object::cast_to<Unit>(
        static_cast<Object*>(all_nodes[i]));
    if (unit == nullptr || !unit->is_inside_tree()) {
      continue;
    }

    if (!damage_caster && unit == caster) {
      continue;
    }

    Vector3 unit_pos = unit->get_global_position();
    float dist_sq = center.distance_squared_to(unit_pos);
    if (dist_sq <= radius_sq) {
      unit->relay(get_take_damage(), base_damage,
                  caster != nullptr ? (godot::Object*)caster : nullptr);
      hit_count++;
    }
  }

  AbilityNode* ability = get_ability();
  String ability_name = ability ? ability->get_ability_name() : "Unknown";
  DBG_INFO("AoEDamageComponent",
           ability_name + " hit " + String::num(hit_count) + " units for " +
               String::num(base_damage, 1) + " damage (radius=" +
               String::num(radius, 1) + ")");
}

void AoEDamageComponent::_collect_units(godot::Node* node,
                                        godot::TypedArray<godot::Node>& out) {
  Unit* unit = Object::cast_to<Unit>(node);
  if (unit != nullptr) {
    out.append(unit);
  }

  for (int i = 0; i < node->get_child_count(); i++) {
    _collect_units(node->get_child(i), out);
  }
}
