#ifndef GDEXTENSION_ABILITY_API_H
#define GDEXTENSION_ABILITY_API_H

#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Array;
using godot::SceneTree;
using godot::Vector3;

class Unit;
namespace godot {
class Node;
}  // namespace godot

/// Static API for ability implementations
/// Common utility functions that abilities can call during execution
///
/// Usage:
/// - AbilityAPI::apply_damage(target, damage, caster)
/// - AbilityAPI::get_units_in_sphere(center, radius)
/// - etc.
///
/// This centralizes common ability logic to reduce code duplication
/// and provide consistent behavior across all abilities
class AbilityAPI {
 public:
  // Damage application
  /// Apply damage to a unit with knockback
  /// Returns actual damage applied (may be reduced by resistances)
  static float apply_damage(Unit* target, float damage, Unit* source = nullptr);

  /// Apply area damage to all units in a sphere
  /// Returns array of units hit
  static Array apply_aoe_damage(const Vector3& center,
                                float radius,
                                float damage,
                                Unit* source = nullptr,
                                Unit* exclude_unit = nullptr);

  // Unit queries
  /// Get all units within a sphere
  /// Returns array of Unit pointers
  static Array get_units_in_sphere(const Vector3& center,
                                   float radius,
                                   Unit* exclude_unit = nullptr);

  /// Get all enemy units within a sphere relative to a reference unit
  /// Returns array of Unit pointers
  static Array get_enemy_units_in_sphere(const Vector3& center,
                                         float radius,
                                         Unit* reference_unit);

  // Status effects / Control
  /// Apply slow effect to unit
  static void apply_slow(Unit* target, float slow_percent, float duration);

  /// Stun a unit (disable movement and abilities)
  static void apply_stun(Unit* target, float duration);

  /// Knockback a unit in a direction
  static void knockback_unit(Unit* target,
                             const Vector3& direction,
                             float force);

  // Utility
  /// Check if two units are enemies
  static bool are_enemies(Unit* unit_a, Unit* unit_b);

  /// Calculate distance between two units
  static float distance_between(Unit* unit_a, Unit* unit_b);

  /// Get the scene tree from any node in the tree
  /// Returns null if node is not in tree or if scene tree is not available
  static godot::SceneTree* get_scene_tree(godot::Node* node);

 private:
  /// Recursive helper to search scene tree for units within radius
  static void _search_units_recursive(godot::Node* node,
                                      const Vector3& center,
                                      float radius_sq,
                                      Unit* exclude_unit,
                                      Array& result);
};

#endif  // GDEXTENSION_ABILITY_API_H
