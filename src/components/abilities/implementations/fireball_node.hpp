#ifndef GDEXTENSION_FIREBALL_NODE_H
#define GDEXTENSION_FIREBALL_NODE_H

#include "../ability_node.hpp"

using godot::String;

class Unit;
class SkillshotProjectile;

/// Fireball - Skillshot projectile ability
/// Launches a fireball that travels from caster towards the clicked position
/// Detonates on impact or max distance, dealing AoE damage to all units in
/// radius
///
/// Properties:
/// - Instant cast (no windup)
/// - Skillshot targeting (fires from caster towards cursor)
/// - Spawns a projectile that travels until it hits a unit or max distance
/// - Applies base_damage to all units in explosion radius
/// - Useful for group damage and requires player aim
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

  // Helper method to spawn projectile
  SkillshotProjectile* _spawn_projectile(Unit* caster,
                                         const godot::Vector3& target_position);
};

#endif  // GDEXTENSION_FIREBALL_NODE_H
