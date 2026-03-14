#include "skillshot_projectile_component.hpp"

#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sphere_shape3d.hpp>
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

// --- Preview interface ---

bool SkillshotProjectileComponent::has_preview() const {
  return max_distance > 0.0f;
}

godot::Node3D* SkillshotProjectileComponent::create_preview() {
  auto* resource_loader = godot::ResourceLoader::get_singleton();
  Ref<godot::PackedScene> scene =
      resource_loader->load("res://previews/skillshot_line.tscn");
  if (scene.is_null()) return nullptr;

  auto* preview = Object::cast_to<godot::Node3D>(scene->instantiate());
  if (preview == nullptr) return nullptr;

  // Read projectile collision radius for width
  float width = 1.0f;
  godot::Node3D* tmpl = _find_template();
  if (tmpl != nullptr) {
    for (int i = 0; i < tmpl->get_child_count(); i++) {
      auto* cs = Object::cast_to<godot::CollisionShape3D>(tmpl->get_child(i));
      if (cs == nullptr || cs->get_shape().is_null()) continue;
      auto sphere = Ref<godot::SphereShape3D>(cs->get_shape());
      if (sphere.is_valid()) {
        width = sphere->get_radius() * 2.0f;
        break;
      }
    }
  }

  // Store width and max_distance as metadata for update_preview to use
  preview->set_meta("line_width", width);
  preview->set_meta("max_distance", max_distance);

  return preview;
}

void SkillshotProjectileComponent::update_preview(
    godot::Node3D* preview, const Vector3& caster_pos,
    const Vector3& ground_pos) {
  if (preview == nullptr) return;

  float width = preview->get_meta("line_width", 1.0f);
  float max_dist = preview->get_meta("max_distance", max_distance);

  // Direction from caster to cursor (horizontal only)
  Vector3 direction = ground_pos - caster_pos;
  direction.y = 0.0f;

  if (direction.length_squared() < 0.01f) return;

  float distance = direction.length();
  if (distance > max_dist) distance = max_dist;
  direction = direction.normalized();

  // Body: centered between caster and endpoint
  // PlaneMesh is 1x1 in XZ, so scale X=width, Z=distance
  godot::Node3D* body = Object::cast_to<godot::Node3D>(
      preview->get_node_or_null(godot::NodePath("Body")));
  godot::Node3D* tip = Object::cast_to<godot::Node3D>(
      preview->get_node_or_null(godot::NodePath("Tip")));

  Vector3 endpoint = caster_pos + direction * distance;
  Vector3 midpoint = (caster_pos + endpoint) * 0.5f;
  midpoint.y = 0.05f;

  // Orient preview to face direction
  preview->set_global_position(caster_pos);
  preview->set_global_position(Vector3(caster_pos.x, 0.05f, caster_pos.z));

  // Calculate rotation angle around Y
  float angle = godot::Math::atan2(-direction.x, -direction.z);
  preview->set_rotation(Vector3(0.0f, angle, 0.0f));

  if (body != nullptr) {
    // Body is a PlaneMesh 1x1. Scale X=width, Z=distance.
    // PlaneMesh lies in XZ plane. Position it so it starts at origin (caster)
    // and extends forward along -Z (Godot's forward).
    body->set_scale(Vector3(width, 1.0f, distance));
    body->set_position(Vector3(0.0f, 0.0f, -distance * 0.5f));
  }

  if (tip != nullptr) {
    // PrismMesh tip at the end of the line
    tip->set_scale(Vector3(width, 1.0f, 1.0f));
    tip->set_position(Vector3(0.0f, 0.0f, -distance));
  }
}
