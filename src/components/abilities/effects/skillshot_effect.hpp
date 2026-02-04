#ifndef GDEXTENSION_SKILLSHOT_EFFECT_H
#define GDEXTENSION_SKILLSHOT_EFFECT_H

#include "../ability_effect.hpp"

using godot::Ref;
using godot::String;

class Unit;
class AbilityDefinition;
class SkillshotProjectile;

/// Skillshot effect - spawns a projectile that travels in a direction
/// Used for directional abilities like Fireball
///
/// The effect:
/// 1. Gets the direction from caster to the clicked point
/// 2. Spawns a SkillshotProjectile at the caster's position
/// 3. Projectile travels and detonates on unit hit or max distance
/// 4. On detonation, applies AoE damage centered at impact point
class SkillshotEffect : public AbilityEffect {
  GDCLASS(SkillshotEffect, AbilityEffect)

 protected:
  static void _bind_methods();

  float projectile_speed = 30.0f;  // Units per second
  float projectile_max_distance =
      20.0f;                           // Max range before projectile detonates
  float explosion_radius = 5.0f;       // AoE radius on detonation
  float projectile_hit_radius = 0.5f;  // Collision detection radius for units

 public:
  SkillshotEffect();
  ~SkillshotEffect();

  /// Execute skillshot - creates projectile traveling toward target location
  void execute(Unit* caster,
               Object* target,
               const AbilityDefinition* ability) override;

  /// Execute skillshot targeting a point
  void execute_at_point(Unit* caster,
                        const Vector3& point,
                        const AbilityDefinition* ability) override;

  // Properties
  void set_projectile_speed(float speed);
  float get_projectile_speed() const;

  void set_projectile_max_distance(float distance);
  float get_projectile_max_distance() const;

  void set_explosion_radius(float radius);
  float get_explosion_radius() const;

  void set_projectile_hit_radius(float radius);
  float get_projectile_hit_radius() const;
};

#endif  // GDEXTENSION_SKILLSHOT_EFFECT_H
