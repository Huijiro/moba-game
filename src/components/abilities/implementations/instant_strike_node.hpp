#ifndef GDEXTENSION_INSTANT_STRIKE_NODE_H
#define GDEXTENSION_INSTANT_STRIKE_NODE_H

#include "../ability_node.hpp"

using godot::String;

class Unit;

/// Instant Strike - Simple single-target instant damage ability
/// Basic attack that immediately damages target unit within range
///
/// Properties:
/// - Instant cast (no windup)
/// - Targets a specific unit
/// - Applies base_damage to target
class InstantStrikeNode : public AbilityNode {
  GDCLASS(InstantStrikeNode, AbilityNode)

 protected:
  static void _bind_methods();

 public:
  InstantStrikeNode();
  ~InstantStrikeNode();

  // Virtual method implementations
  bool execute(Unit* caster,
               Unit* target,
               godot::Vector3 position = godot::Vector3()) override;

  bool can_execute_on_target(Unit* caster, Unit* target) const override;

  float calculate_damage(Unit* caster, Unit* target = nullptr) const override;
};

#endif  // GDEXTENSION_INSTANT_STRIKE_NODE_H
