#ifndef GDEXTENSION_BEAM_NODE_H
#define GDEXTENSION_BEAM_NODE_H

#include "../ability_node.hpp"

using godot::String;

class Unit;

/// Energy Beam - Channel-based continuous damage ability
/// Requires player to hold button to maintain the beam
/// Applies damage to target unit in ticks while channeling
///
/// Properties:
/// - Channel cast (must hold button to maintain)
/// - Targets a specific unit
/// - Applies base_damage per tick at channel_tick_interval
/// - Total damage = base_damage * (channel_duration / channel_tick_interval)
/// - Channeling can be interrupted if player releases button
class BeamNode : public AbilityNode {
  GDCLASS(BeamNode, AbilityNode)

 protected:
  static void _bind_methods();

 public:
  BeamNode();
  ~BeamNode();

  // Virtual method implementations
  void execute(Unit* caster,
               Unit* target,
               godot::Vector3 position = godot::Vector3()) override;

  bool can_execute_on_target(Unit* caster, Unit* target) const override;

  float calculate_damage(Unit* caster, Unit* target = nullptr) const override;
};

#endif  // GDEXTENSION_BEAM_NODE_H
