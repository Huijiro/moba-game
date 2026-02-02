#ifndef GDEXTENSION_FIREBALL_NODE_H
#define GDEXTENSION_FIREBALL_NODE_H

#include "../ability_node.hpp"

using godot::String;

class Unit;

/// Fireball - Area-of-effect instant damage ability
/// Projectile/instant ability that damages all units in a radius
/// Targets a point in the world and applies damage to all units in AoE
///
/// Properties:
/// - Instant cast (no windup)
/// - Targets an area (all units within aoe_radius)
/// - Applies base_damage to all units in the area
/// - Useful for group damage and zoning
class FireballNode : public AbilityNode {
  GDCLASS(FireballNode, AbilityNode)

 protected:
  static void _bind_methods();

 public:
  FireballNode();
  ~FireballNode();

  // Virtual method implementations
  void execute(Unit* caster,
               Unit* target,
               godot::Vector3 position = godot::Vector3()) override;

  bool can_execute_on_target(Unit* caster, Unit* target) const override;

  float calculate_damage(Unit* caster, Unit* target = nullptr) const override;

  // Helper method to find all units in area
  godot::Array query_units_in_area(const godot::Vector3& center) const;

 private:
  // Recursive helper to search scene tree for units within radius
  void _search_units_in_tree(godot::Node* node,
                             const godot::Vector3& center,
                             float radius_sq,
                             godot::Array& result) const;
};

#endif  // GDEXTENSION_FIREBALL_NODE_H
