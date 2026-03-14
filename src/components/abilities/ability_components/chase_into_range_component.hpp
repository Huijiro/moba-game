#ifndef CHASE_INTO_RANGE_COMPONENT_HPP
#define CHASE_INTO_RANGE_COMPONENT_HPP

#include "../ability_subcomponent.hpp"

class Unit;

// ChaseIntoRangeComponent encodes the behavior where a unit automatically
// moves toward a target until the target is within ability range, at which point
// the unit stops and attacks.
//
// This component is used by abilities that have unit targeting and a range.
// It validates that the unit is in range before allowing ability execution.
class ChaseIntoRangeComponent : public AbilitySubcomponent {
  GDCLASS(ChaseIntoRangeComponent, AbilitySubcomponent);

 public:
  ChaseIntoRangeComponent();
  ~ChaseIntoRangeComponent();

  // Validate that a given target is within range of the ability.
  // If not in range, the unit should chase the target.
  // Returns true if target is in range and ability can execute.
  bool validate_target_in_range(Unit* caster, Unit* target) const;

 protected:
  static void _bind_methods();
};

#endif  // CHASE_INTO_RANGE_COMPONENT_HPP
