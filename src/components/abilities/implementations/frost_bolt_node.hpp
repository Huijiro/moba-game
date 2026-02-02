#ifndef GDEXTENSION_FROST_BOLT_NODE_H
#define GDEXTENSION_FROST_BOLT_NODE_H

#include "../ability_node.hpp"

using godot::String;

class Unit;

/// Frost Bolt - Single-target cast-time damage ability with slow effect
/// Ranged attack that has a windup time before executing
/// Applies damage to target unit and can apply slow effect
///
/// Properties:
/// - Cast time ability (0.7s windup)
/// - Targets a specific unit
/// - Applies base_damage to target
/// - Cast point at 0.5 means ability fires at 50% through cast time
class FrostBoltNode : public AbilityNode {
  GDCLASS(FrostBoltNode, AbilityNode)

 protected:
  static void _bind_methods();

 public:
  FrostBoltNode();
  ~FrostBoltNode();

  // Virtual method implementations
  void execute(Unit* caster,
               Unit* target,
               godot::Vector3 position = godot::Vector3()) override;

  bool can_execute_on_target(Unit* caster, Unit* target) const override;

  float calculate_damage(Unit* caster, Unit* target = nullptr) const override;
};

#endif  // GDEXTENSION_FROST_BOLT_NODE_H
