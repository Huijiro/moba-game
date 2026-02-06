#include "fireball_node.hpp"
#include "../../../debug/debug_macros.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../core/unit.hpp"
#include "../../combat/skillshot_projectile.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Node;
using godot::String;
using godot::UtilityFunctions;
using godot::Vector3;

FireballNode::FireballNode() {
  // Set sensible defaults for fireball
  set_ability_name("Fireball");
  set_description("Launch a fireball projectile that hits on collision");
  set_cast_type(static_cast<int>(CastType::INSTANT));
  set_targeting_type(static_cast<int>(TargetingType::SKILLSHOT));
  set_base_damage(50.0f);
  set_range(20.0f);
  set_cooldown(7.0f);
}

FireballNode::~FireballNode() = default;

void FireballNode::_bind_methods() {
  // No custom properties for FireballNode, inherits all from AbilityNode
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "position"),
                       &FireballNode::execute);
  ClassDB::bind_method(D_METHOD("can_execute_on_target", "caster", "target"),
                       &FireballNode::can_execute_on_target);
  ClassDB::bind_method(D_METHOD("calculate_damage", "caster", "target"),
                       &FireballNode::calculate_damage);
}

bool FireballNode::execute(Unit* caster, Unit* target, Vector3 position) {
  if (caster == nullptr) {
    DBG_INFO("Fireball", "No caster provided");
    return false;
  }

  // For skillshot, position is where the projectile should travel to
  // If no position provided, use a default forward direction
  Vector3 target_position = position;
  if (target_position == Vector3(0, 0, 0)) {
    // Use caster's forward direction
    target_position =
        caster->get_global_position() +
        caster->get_global_transform().basis.get_column(2) * get_range();
  }

  // Spawn the projectile
  SkillshotProjectile* projectile = _spawn_projectile(caster, target_position);
  if (projectile != nullptr) {
    DBG_INFO("Fireball", "Launched fireball from " + caster->get_name() +
                             " towards (" + String::num(target_position.x, 2) +
                             ", " + String::num(target_position.z, 2) + ")");
    return true;
  }
  return false;
}

bool FireballNode::can_execute_on_target(Unit* caster, Unit* target) const {
  if (caster == nullptr) {
    return false;
  }

  // For skillshot abilities, we just need a valid caster
  // Target position will be provided during execution
  return true;
}

float FireballNode::calculate_damage(Unit* caster, Unit* target) const {
  // Simple damage: just return base damage
  // Subclasses can override to add scaling (e.g., based on caster stats)
  return get_base_damage();
}

SkillshotProjectile* FireballNode::_spawn_projectile(
    Unit* caster,
    const Vector3& target_position) {
  if (caster == nullptr || !caster->is_inside_tree()) {
    DBG_INFO("Fireball", "Cannot spawn projectile: caster invalid");
    return nullptr;
  }

  // Create new projectile node
  SkillshotProjectile* projectile = memnew(SkillshotProjectile);
  if (projectile == nullptr) {
    DBG_INFO("Fireball", "Failed to create projectile");
    return nullptr;
  }

  // Add projectile to scene tree (as child of caster's parent for easier
  // management)
  Node* parent = caster->get_parent();
  if (parent != nullptr) {
    parent->add_child(projectile);
  } else {
    DBG_INFO("Fireball", "Warning: Caster has no parent");
    // Don't add to tree if parent doesn't exist
    delete projectile;
    return nullptr;
  }

  // Calculate direction from caster to target
  Vector3 caster_pos = caster->get_global_position();
  Vector3 direction = target_position - caster_pos;

  // Setup the projectile
  projectile->setup(
      caster,     // Caster unit
      direction,  // Direction vector (will be normalized)
      calculate_damage(caster, nullptr),  // Damage amount
      get_skillshot_speed(),              // Speed
      get_skillshot_max_distance(),       // Max distance (range)
      0.0f,                               // No AoE (single target)
      get_skillshot_hit_radius()          // Hit radius (collision detection)
  );

  return projectile;
}
