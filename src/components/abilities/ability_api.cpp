#include "ability_api.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../core/unit.hpp"
#include "../health/health_component.hpp"
#include "../../debug/debug_macros.hpp"

using godot::Node;
using godot::Object;
using godot::SceneTree;
using godot::String;
using godot::UtilityFunctions;
using godot::Vector3;

float AbilityAPI::apply_damage(Unit* target, float damage, Unit* source) {
  if (target == nullptr || !target->is_inside_tree()) {
    return 0.0f;
  }

  HealthComponent* health = Object::cast_to<HealthComponent>(
      target->get_component_by_class("HealthComponent"));

  if (health == nullptr) {
    return 0.0f;
  }

  health->apply_damage(damage, source);
  return damage;
}

Array AbilityAPI::apply_aoe_damage(const Vector3& center,
                                   float radius,
                                   float damage,
                                   Unit* source,
                                   Unit* exclude_unit) {
  Array hit_units = get_units_in_sphere(center, radius, exclude_unit);
  Array result;

  for (int i = 0; i < hit_units.size(); i++) {
    Unit* unit = Object::cast_to<Unit>(hit_units[i]);
    if (unit == nullptr || unit == source) {
      continue;
    }

    float applied_damage = apply_damage(unit, damage, source);
    if (applied_damage > 0.0f) {
      result.append(unit);
    }
  }

  return result;
}

Array AbilityAPI::get_units_in_sphere(const Vector3& center,
                                      float radius,
                                      Unit* exclude_unit) {
  Array result;

  // Find all Unit nodes within radius using scene tree traversal
  // This is a brute-force approach but works for smaller scenes
  // For larger scenes, consider using physics queries or spatial partitioning

  // Get a node to work with - use exclude_unit if available
  Node* node = exclude_unit;
  if (node == nullptr || !node->is_inside_tree()) {
    return result;
  }

  // Get the scene tree using Godot's built-in method
  SceneTree* scene_tree = get_scene_tree(node);
  if (scene_tree == nullptr) {
    return result;
  }

  // Find the root by traversing up from exclude_unit
  Node* current = exclude_unit;
  Node* root = exclude_unit;
  while (current != nullptr) {
    root = current;
    current = current->get_parent();
  }

  // Traverse from root
  float radius_sq = radius * radius;
  _search_units_recursive(root, center, radius_sq, exclude_unit, result);

  return result;
}

SceneTree* AbilityAPI::get_scene_tree(Node* node) {
  if (node == nullptr || !node->is_inside_tree()) {
    return nullptr;
  }

  // Use Godot's built-in get_tree() method to get the scene tree
  return node->get_tree();
}

Array AbilityAPI::get_enemy_units_in_sphere(const Vector3& center,
                                            float radius,
                                            Unit* reference_unit) {
  Array all_units = get_units_in_sphere(center, radius, reference_unit);
  Array enemies;

  for (int i = 0; i < all_units.size(); i++) {
    Unit* unit = Object::cast_to<Unit>(all_units[i]);
    if (unit == nullptr) {
      continue;
    }

    if (are_enemies(reference_unit, unit)) {
      enemies.append(unit);
    }
  }

  return enemies;
}

void AbilityAPI::apply_slow(Unit* target, float slow_percent, float duration) {
  if (target == nullptr || !target->is_inside_tree()) {
    return;
  }

  // TODO: Implement status effect system
  // For now, this is a placeholder
  // Would apply movement speed reduction for duration seconds
  DBG_INFO("AbilityAPI", "Slow applied (not yet implemented)");
}

void AbilityAPI::apply_stun(Unit* target, float duration) {
  if (target == nullptr || !target->is_inside_tree()) {
    return;
  }

  // TODO: Implement status effect system
  // For now, this is a placeholder
  // Would disable movement and ability casting for duration seconds
  DBG_INFO("AbilityAPI", "Stun applied (not yet implemented)");
}

void AbilityAPI::knockback_unit(Unit* target,
                                const Vector3& direction,
                                float force) {
  if (target == nullptr || !target->is_inside_tree()) {
    return;
  }

  // TODO: Implement knockback physics
  // For now, this is a placeholder
  // Would apply velocity in direction for force amount
  DBG_INFO("AbilityAPI", "Knockback applied (not yet implemented)");
}

bool AbilityAPI::are_enemies(Unit* unit_a, Unit* unit_b) {
  if (unit_a == nullptr || unit_b == nullptr) {
    return false;
  }

  // TODO: Implement team/faction system
  // For now, assume all units are enemies except themselves
  return unit_a != unit_b;
}

float AbilityAPI::distance_between(Unit* unit_a, Unit* unit_b) {
  if (unit_a == nullptr || unit_b == nullptr) {
    return 0.0f;
  }

  return unit_a->get_global_position().distance_to(
      unit_b->get_global_position());
}

void AbilityAPI::_search_units_recursive(Node* node,
                                         const Vector3& center,
                                         float radius_sq,
                                         Unit* exclude_unit,
                                         Array& result) {
  if (node == nullptr) {
    return;
  }

  // Check if current node is a Unit
  Unit* unit = Object::cast_to<Unit>(node);
  if (unit != nullptr && unit != exclude_unit && unit->is_inside_tree()) {
    Vector3 unit_pos = unit->get_global_position();
    float distance_sq = center.distance_squared_to(unit_pos);

    if (distance_sq <= radius_sq) {
      result.append(unit);
    }
  }

  // Recursively search children
  for (int i = 0; i < node->get_child_count(); i++) {
    Node* child = node->get_child(i);
    if (child != nullptr) {
      _search_units_recursive(child, center, radius_sq, exclude_unit, result);
    }
  }
}
