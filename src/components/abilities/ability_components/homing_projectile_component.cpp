#include "homing_projectile_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../../combat/projectile.hpp"
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

HomingProjectileComponent::HomingProjectileComponent() = default;

HomingProjectileComponent::~HomingProjectileComponent() = default;

void HomingProjectileComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_damage", "damage"),
                       &HomingProjectileComponent::set_damage);
  ClassDB::bind_method(D_METHOD("get_damage"),
                       &HomingProjectileComponent::get_damage);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "damage"), "set_damage",
               "get_damage");

  ClassDB::bind_method(D_METHOD("set_speed", "speed"),
                       &HomingProjectileComponent::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"),
                       &HomingProjectileComponent::get_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed",
               "get_speed");

  ClassDB::bind_method(D_METHOD("_on_execute", "context"),
                       &HomingProjectileComponent::_on_execute);
}

void HomingProjectileComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("execute", godot::Callable(this, "_on_execute"));
  }

  // Hide the projectile template child
  godot::Node3D* tmpl = _find_template();
  if (tmpl != nullptr) {
    tmpl->set_visible(false);
  }
}

godot::Node3D* HomingProjectileComponent::_find_template() const {
  for (int i = 0; i < get_child_count(); i++) {
    godot::Node3D* child = Object::cast_to<godot::Node3D>(get_child(i));
    if (child != nullptr) {
      return child;
    }
  }
  return nullptr;
}

void HomingProjectileComponent::set_damage(float d) { damage = d; }
float HomingProjectileComponent::get_damage() const { return damage; }

void HomingProjectileComponent::set_speed(float s) { speed = s; }
float HomingProjectileComponent::get_speed() const { return speed; }

void HomingProjectileComponent::_on_execute(const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) return;

  Unit* caster = ctx->get_caster();
  Unit* target = ctx->get_target();

  if (caster == nullptr || !caster->is_inside_tree()) {
    DBG_WARN("HomingProjectile", "No valid caster");
    return;
  }

  if (target == nullptr || !target->is_inside_tree()) {
    DBG_WARN("HomingProjectile", "No valid target for homing projectile");
    return;
  }

  // Find and duplicate the template child
  godot::Node3D* template_node = _find_template();
  if (template_node == nullptr) {
    DBG_WARN("HomingProjectile", "No projectile template child found");
    return;
  }

  godot::Node* instance = template_node->duplicate();
  if (instance == nullptr) {
    DBG_WARN("HomingProjectile", "Failed to duplicate template");
    return;
  }

  // Add to scene
  godot::Node* scene_parent = caster->get_parent();
  if (scene_parent == nullptr) {
    instance->queue_free();
    return;
  }

  scene_parent->add_child(instance);

  auto* node3d = Object::cast_to<godot::Node3D>(instance);
  if (node3d != nullptr) {
    node3d->set_visible(true);
    node3d->set_global_position(caster->get_global_position() +
                                Vector3(0, 1, 0));
  }

  // Setup as homing projectile
  Projectile* projectile = Object::cast_to<Projectile>(instance);
  if (projectile != nullptr) {
    projectile->setup(caster, target, damage, speed);
    AbilityNode* ability = get_ability();
    DBG_INFO("HomingProjectile",
             (ability ? ability->get_ability_name() : String("Unknown")) +
                 " launched at " + target->get_name());
  } else {
    DBG_WARN("HomingProjectile",
             "Template child is not a Projectile node");
  }
}
