#ifndef GDEXTENSION_ABILITY_TYPES_H
#define GDEXTENSION_ABILITY_TYPES_H

// Ability casting type - determines execution behavior
enum class CastType {
  INSTANT,    // Ability executes immediately
  CAST_TIME,  // Ability has windup/casting time before execution
  CHANNEL     // Ability requires player to hold button to maintain
};

// Ability targeting paradigm - determines what the ability targets
enum class TargetingType {
  UNIT_TARGET,   // Must target a specific unit (enemy/ally/self)
  POINT_TARGET,  // Targets a point in the world
  AREA,          // Affects all units in an area around a point
  SKILLSHOT,     // Directional ability (line, cone, etc.) - future
  SELF_CAST      // No targeting required, affects caster
};

// Ability casting state - tracks where we are in the casting sequence
enum class CastState {
  IDLE,        // Ready to cast or off cooldown
  CASTING,     // Currently in cast time, waiting for cast_point
  CHANNELING,  // Channeling ability, waiting for player to release
  ON_COOLDOWN  // Recently cast, waiting for cooldown to expire
};

#endif  // GDEXTENSION_ABILITY_TYPES_H
