#include "skillshot_projectile_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../../combat/skillshot_projectile.hpp"
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

SkillshotProjectileComponent::SkillshotProjectileComponent() = default;

SkillshotProjectileComponent::~SkillshotProjectileComponent() = default;

void SkillshotProjectileComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_damage", "damage"),
                       &SkillshotProjectileComponent::set_damage);
  ClassDB::bind_method(D_METHOD("get_damage"),
                       &SkillshotProjectileComponent::get_damage);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "damage"), "set_damage",
               "get_damage");

  ClassDB::bind_method(D_METHOD("set_speed", "speed"),
                       &SkillshotProjectileComponent::set_speed);
  ClassDB::bind_method(D_METHOD("get_speed"),
                       &SkillshotProjectileComponent::get_speed);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed",
               "get_speed");

  ClassDB::bind_method(D_METHOD("set_max_distance", "distance"),
                       &SkillshotProjectileComponent::set_max_distance);
  ClassDB::bind_method(D_METHOD("get_max_distance"),
                       &SkillshotProjectileComponent::get_max_distance);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_distance"),
               "set_max_distance", "get_max_distance");

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
    ability->connect("execute", godot::Callable(this, "_on_execute"));
  }

  // Hide the projectile template child
  godot::Node3D* tmpl = _find_template();
  if (tmpl != nullptr) {
    tmpl->set_visible(false);
  }
}

godot::Node3D* SkillshotProjectileComponent::_find_template() const {
  for (int i = 0; i < get_child_count(); i++) {
    godot::Node3D* child = Object::cast_to<godot::Node3D>(get_child(i));
    if (child != nullptr) {
      return child;
    }
  }
  return nullptr;
}

void SkillshotProjectileComponent::set_damage(float d) { damage = d; }
float SkillshotProjectileComponent::get_damage() const { return damage; }

void SkillshotProjectileComponent::set_speed(float s) { speed = s; }
float SkillshotProjectileComponent::get_speed() const { return speed; }

void SkillshotProjectileComponent::set_max_distance(float d) {
  max_distance = d;
}
float SkillshotProjectileComponent::get_max_distance() const {
  return max_distance;
}

void SkillshotProjectileComponent::_on_execute(
    const Ref<RefCounted>& context) {
  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) return;

  Unit* caster = ctx->get_caster();
  if (caster == nullptr || !caster->is_inside_tree()) {
    DBG_WARN("SkillshotProjectile", "No valid caster");
    return;
  }

  // Determine direction toward target
  Vector3 caster_pos = caster->get_global_position();
  Vector3 target_pos = ctx->get_position();

  Unit* target_unit = ctx->get_target();
  if (target_unit != nullptr && target_unit->is_inside_tree()) {
    target_pos = target_unit->get_global_position();
  }

  Vector3 direction;
  if (target_pos != Vector3()) {
    direction = (target_pos - caster_pos).normalized();
  } else {
    direction = -caster->get_global_transform().basis.get_column(2);
  }

  // Find and duplicate the template child
  godot::Node3D* template_node = _find_template();
  if (template_node == nullptr) {
    DBG_WARN("SkillshotProjectile", "No projectile template child found");
    return;
  }

  godot::Node* instance = template_node->duplicate();
  if (instance == nullptr) {
    DBG_WARN("SkillshotProjectile", "Failed to duplicate template");
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
    node3d->set_global_position(caster_pos + Vector3(0, 1, 0));
  }

  // Setup as skillshot
  SkillshotProjectile* projectile =
      Object::cast_to<SkillshotProjectile>(instance);
  if (projectile != nullptr) {
    projectile->setup(caster, direction, speed, max_distance);

    projectile->connect("hit", godot::Callable(this, "_on_projectile_hit"));
    projectile->connect("reached_max_range",
                        godot::Callable(this, "_on_projectile_max_range"));

    AbilityNode* ability = get_ability();
    DBG_INFO("SkillshotProjectile",
             (ability ? ability->get_ability_name() : String("Unknown")) +
                 " launched toward (" + String::num(direction.x, 2) + ", " +
                 String::num(direction.z, 2) + ")");
  } else {
    DBG_WARN("SkillshotProjectile",
             "Template child is not a SkillshotProjectile node");
  }
}

void SkillshotProjectileComponent::_on_projectile_hit(
    Object* unit, const Vector3& position) {
  Unit* hit_unit = Object::cast_to<Unit>(unit);
  if (hit_unit == nullptr || !hit_unit->is_inside_tree()) return;

  if (damage > 0.0f) {
    hit_unit->relay(get_take_damage(), damage, (Object*)nullptr);
  }
}

void SkillshotProjectileComponent::_on_projectile_max_range(
    const Vector3& position) {
  // Skillshot missed — could trigger AoE effects here in the future
}
