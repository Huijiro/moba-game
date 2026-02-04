#ifndef GDEXTENSION_EXPLOSION_NODE_H
#define GDEXTENSION_EXPLOSION_NODE_H

#include "../ability_node.hpp"

using godot::String;

class Unit;

/// Explosion - Instant self-cast AoE damage ability
/// Damages all units in a radius around the caster
/// No targeting required - affects area around caster
///
/// Properties:
/// - Instant cast (no windup)
/// - Self-cast (no target selection needed)
/// - Targets all units in area around caster
/// - Applies base_damage to all units in radius
class ExplosionNode : public AbilityNode {
  GDCLASS(ExplosionNode, AbilityNode)

 protected:
  static void _bind_methods();

 public:
  ExplosionNode();
  ~ExplosionNode();

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

#endif  // GDEXTENSION_EXPLOSION_NODE_H
