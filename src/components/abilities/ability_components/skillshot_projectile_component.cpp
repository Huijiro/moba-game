#include "skillshot_projectile_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../../combat/skillshot_projectile.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::String;
using godot::Variant;
using godot::Vector3;

SkillshotProjectileComponent::SkillshotProjectileComponent() = default;

SkillshotProjectileComponent::~SkillshotProjectileComponent() = default;

void SkillshotProjectileComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_damage", "damage"),
                       &SkillshotProjectileComponent::set_damage);
  ClassDB::bind_method(D_METHOD("get_damage"),
                       &SkillshotProjectileComponent::get_damage);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "damage"),
               "set_damage", "get_damage");

  ClassDB::bind_method(D_METHOD("set_speed", "speed"),
                       &SkillshotProjectileComponent::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"),
                       &SkillshotProjectileComponent::get_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"),
               "set_speed", "get_speed");

  ClassDB::bind_method(D_METHOD("set_max_distance", "distance"),
                       &SkillshotProjectileComponent::set_max_distance);
  ClassDB::bind_method(D_METHOD("get_max_distance"),
                       &SkillshotProjectileComponent::get_max_distance);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_distance"),
               "set_max_distance", "get_max_distance");

  ClassDB::bind_method(D_METHOD("set_hit_radius", "radius"),
                       &SkillshotProjectileComponent::set_hit_radius);
  ClassDB::bind_method(D_METHOD("get_hit_radius"),
                       &SkillshotProjectileComponent::get_hit_radius);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hit_radius"),
               "set_hit_radius", "get_hit_radius");

  ClassDB::bind_method(D_METHOD("set_projectile_node_name", "name"),
                       &SkillshotProjectileComponent::set_projectile_node_name);
  ClassDB::bind_method(D_METHOD("get_projectile_node_name"),
                       &SkillshotProjectileComponent::get_projectile_node_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "projectile_node_name"),
               "set_projectile_node_name", "get_projectile_node_name");

  ClassDB::bind_method(D_METHOD("_on_execute", "context"),
                       &SkillshotProjectileComponent::_on_execute);
  ClassDB::bind_method(D_METHOD("_on_projectile_hit", "unit", "position"),
                       &SkillshotProjectileComponent::_on_projectile_hit);
  ClassDB::bind_method(D_METHOD("_on_projectile_max_range", "position"),
                       &SkillshotProjectileComponent::_on_projectile_max_range);
}

void SkillshotProjectileComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("execute",
                     godot::Callable(this, "_on_execute"));
  }
}

void SkillshotProjectileComponent::set_damage(float d) { damage = d; }
float SkillshotProjectileComponent::get_damage() const { return damage; }

void SkillshotProjectileComponent::set_speed(float s) { speed = s; }
float SkillshotProjectileComponent::get_speed() const { return speed; }

void SkillshotProjectileComponent::set_max_distance(float d) { max_distance = d; }
float SkillshotProjectileComponent::get_max_distance() const { return max_distance; }

void SkillshotProjectileComponent::set_hit_radius(float r) { hit_radius = r; }
float SkillshotProjectileComponent::get_hit_radius() const { return hit_radius; }

void SkillshotProjectileComponent::set_projectile_node_name(const String& name) {
  projectile_node_name = name;
}

String SkillshotProjectileComponent::get_projectile_node_name() const {
  return projectile_node_name;
}

void SkillshotProjectileComponent::_on_execute(
    const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) {
    return;
  }

  Unit* caster = ctx->get_caster();
  if (caster == nullptr || !caster->is_inside_tree()) {
    DBG_WARN("SkillshotProjectileComponent", "No valid caster");
    return;
  }

  // Determine direction: toward target position, or caster's forward
  Vector3 caster_pos = caster->get_global_position();
  Vector3 target_pos = ctx->get_position();

  // If we have a unit target, use their position as aim point
  Unit* target_unit = ctx->get_target();
  if (target_unit != nullptr && target_unit->is_inside_tree()) {
    target_pos = target_unit->get_global_position();
  }

  Vector3 direction;
  if (target_pos != Vector3()) {
    direction = (target_pos - caster_pos).normalized();
  } else {
    // Fall back to caster's forward direction (-Z)
    direction = -caster->get_global_transform().basis.get_column(2);
  }

  // Find projectile template on the AbilityNode
  AbilityNode* ability = get_ability();
  if (ability == nullptr) {
    return;
  }

  godot::Node* template_node = ability->find_child(projectile_node_name, false, false);
  if (template_node == nullptr) {
    DBG_WARN("SkillshotProjectileComponent",
             "Projectile template '" + projectile_node_name + "' not found");
    return;
  }

  // Duplicate the template
  godot::Node* instance = template_node->duplicate();
  if (instance == nullptr) {
    DBG_WARN("SkillshotProjectileComponent", "Failed to duplicate projectile template");
    return;
  }

  // Add to scene as sibling of caster
  godot::Node* scene_parent = caster->get_parent();
  if (scene_parent == nullptr) {
    instance->queue_free();
    return;
  }

  scene_parent->add_child(instance);

  // Position at caster
  auto* node3d = Object::cast_to<godot::Node3D>(instance);
  if (node3d != nullptr) {
    node3d->set_global_position(caster_pos + Vector3(0, 1, 0));
  }

  // Setup as skillshot projectile
  SkillshotProjectile* projectile = Object::cast_to<SkillshotProjectile>(instance);
  if (projectile != nullptr) {
    projectile->setup(caster, direction, speed, max_distance, hit_radius);

    // Connect to projectile signals — we handle what happens on hit
    projectile->connect("hit",
                        godot::Callable(this, "_on_projectile_hit"));
    projectile->connect("reached_max_range",
                        godot::Callable(this, "_on_projectile_max_range"));

    DBG_INFO("SkillshotProjectileComponent",
             ability->get_ability_name() + " launched skillshot toward (" +
                 String::num(direction.x, 2) + ", " +
                 String::num(direction.z, 2) + ")");
  } else {
    DBG_WARN("SkillshotProjectileComponent",
             "Template '" + projectile_node_name +
                 "' is not a SkillshotProjectile node");
  }
}

void SkillshotProjectileComponent::_on_projectile_hit(
    Object* unit, const Vector3& position) {
  Unit* hit_unit = Object::cast_to<Unit>(unit);
  if (hit_unit == nullptr || !hit_unit->is_inside_tree()) {
    return;
  }

  if (damage > 0.0f) {
    hit_unit->relay(get_take_damage(), damage, (Object*)nullptr);
    AbilityNode* ability = get_ability();
    String ability_name = ability ? ability->get_ability_name() : "Unknown";
    DBG_INFO("SkillshotProjectileComponent",
             ability_name + " hit " + hit_unit->get_name() + " for " +
                 String::num(damage, 1) + " damage");
  }
}

void SkillshotProjectileComponent::_on_projectile_max_range(
    const Vector3& position) {
  // Skillshot missed — reached max range without hitting anything.
  // Could trigger effects here in the future (e.g., spawn AoE at position).
  AbilityNode* ability = get_ability();
  String ability_name = ability ? ability->get_ability_name() : "Unknown";
  DBG_DEBUG("SkillshotProjectileComponent",
            ability_name + " reached max range at (" +
                String::num(position.x, 1) + ", " +
                String::num(position.z, 1) + ")");
}
