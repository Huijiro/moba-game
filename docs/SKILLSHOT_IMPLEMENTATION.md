# Skillshot Implementation Guide

## Overview

Skillshot abilities have been fully implemented in the MOBA game. A skillshot is a directional ability that:

1. **Activates on ability press** (Q/W/E/R key)
2. **Enters targeting mode** - awaits player click to determine direction
3. **Spawns projectile** - travels in direction from caster to click location
4. **Detonates on impact** - hits first enemy unit OR max distance reached
5. **Applies AoE damage** - all units in explosion radius take damage

## Architecture

### New Classes

#### SkillshotProjectile (`src/components/combat/skillshot_projectile.hpp/cpp`)
- Inherits: `Node3D`
- Travels in a fixed direction
- Detects collision with units
- Applies AoE damage on detonation
- Handles max distance checks

**Key Methods:**
- `setup()` - Initialize projectile with all parameters
- `_physics_process()` - Handle movement and collision detection
- `_detonate()` - Trigger explosion at current location
- `_find_and_damage_units()` - Find and damage all units in AoE radius

**Properties:**
- `speed` - Projectile travel speed (units/sec, default: 30)
- `max_distance` - Max range before detonation (default: 20)
- `hit_radius` - Collision detection radius (default: 0.5)
- `aoe_radius` - Explosion damage radius (default: 5.0)

#### SkillshotEffect (`src/components/abilities/effects/skillshot_effect.hpp/cpp`)
- Inherits: `AbilityEffect`
- Creates and spawns SkillshotProjectiles
- Calculates direction from caster to target/point
- Handles both unit-targeted and point-targeted skillshots

**Key Methods:**
- `execute()` - Create skillshot toward a unit target
- `execute_at_point()` - Create skillshot toward a clicked point

**Properties:**
- `projectile_speed` - Speed of spawned projectile
- `projectile_max_distance` - Max range for spawned projectile
- `explosion_radius` - AoE radius on detonation
- `projectile_hit_radius` - Collision detection radius

### Updated Classes

#### TargetingType Enum (`src/components/abilities/ability_types.hpp`)
Added: `SKILLSHOT = 4` (was previously planned as "future")

#### AbilityDefinition (`src/components/abilities/ability_definition.hpp/cpp`)
New properties for skillshot configuration:
- `skillshot_speed` - Projectile speed (editor property)
- `skillshot_max_distance` - Max range (editor property)
- `skillshot_hit_radius` - Collision radius (editor property)

#### InputManager (`src/input/input_manager.cpp`)
- Skillshot targeting automatically treated as position-based targeting
- Press ability key → awaits position click → spawns projectile
- No additional changes needed (falls through to POINT_TARGET/AREA handling)

#### AbilityComponent (`src/components/abilities/ability_component.cpp`)
- No changes needed
- Already routes position-based abilities (POINT_TARGET=1, AREA=2) through `execute_at_point()`
- Added SKILLSHOT (4) to this routing group for consistency

## Usage - Creating a Skillshot Ability

### Step 1: Create the Effect Resource
Create `res://resources/abilities/MySkillshotEffect.tres`:

```
[gd_resource type="SkillshotEffect" format=3]

[resource]
projectile_speed = 30.0
projectile_max_distance = 20.0
explosion_radius = 5.0
projectile_hit_radius = 0.8
```

### Step 2: Create the Ability Definition
Create `res://resources/abilities/MySkillshot.tres`:

```
[gd_resource type="AbilityDefinition" format=3]
[ext_resource type="Resource" path="res://resources/abilities/MySkillshotEffect.tres" id="1_effect"]

[resource]
ability_name = "My Skillshot"
description = "A skillshot ability that deals damage"
mana_cost = 50.0
cooldown = 8.0
cast_type = 0
cast_time = 0.0
cast_point = 0.0
channel_duration = 0.0
targeting_type = 4
range = 20.0
aoe_radius = 5.0
skillshot_speed = 30.0
skillshot_max_distance = 20.0
skillshot_hit_radius = 0.8
base_damage = 40.0
effect = ExtResource("1_effect")
```

**Key Settings:**
- `targeting_type = 4` (SKILLSHOT)
- `cast_type = 0` (INSTANT) - No cast time, fires immediately
- `skillshot_speed` - How fast projectile travels
- `skillshot_max_distance` - How far it goes before detonating
- `skillshot_hit_radius` - Hit detection radius (collision box)
- `aoe_radius` - Explosion damage radius

### Step 3: Add to Unit Definition
In your unit's abilities array, set the skillshot ability to one of the slots (Q/W/E/R).

## Current Implementation - W Fireball

**File:** `GodotGame/resources/abilities/W_Fireball.tres`

Configuration:
- **Type:** SKILLSHOT (targeting_type = 4)
- **Cast:** INSTANT (cast_type = 0)
- **Mana Cost:** 50
- **Cooldown:** 8 seconds
- **Speed:** 25 units/sec
- **Max Distance:** 20 units
- **Damage:** 40 damage to all units in 5m radius
- **Collision Radius:** 0.8 units

**How to Test:**
1. Run the game
2. Press W (or click W button)
3. Click on ground to aim direction
4. Projectile travels toward click location
5. Explodes on unit hit or after 20 units traveled
6. All units in 5m radius take 40 damage

## Gameplay Flow

```
Player presses W
    ↓
InputManager enters targeting mode
    ↓
[Waiting for player click]
    ↓
Player clicks on location
    ↓
InputManager calls ability_component->try_cast_point()
    ↓
AbilityComponent calls SkillshotEffect->execute_at_point()
    ↓
SkillshotEffect creates SkillshotProjectile and spawns it
    ↓
[Projectile travels toward target]
    ↓
(A) Hit unit → Detonate immediately
(B) Max distance reached → Detonate
    ↓
SkillshotProjectile applies AoE damage centered at detonation point
    ↓
All enemies in AoE radius take damage
```

## Technical Details

### Collision Detection
The projectile checks for unit collisions every frame by:
1. Starting from caster's parent node
2. Walking the node tree searching for Units
3. Checking distance from current projectile position to each unit
4. If distance ≤ hit_radius, trigger detonation

### AoE Damage
On detonation, the projectile:
1. Searches the node tree for all Units
2. Calculates distance from explosion center to each unit
3. If distance ≤ aoe_radius, applies damage
4. Skips the caster itself

### Direction Calculation
Direction is determined by:
- **Point-target skillshots:** (click_point - caster_position).normalized()
- **Unit-target skillshots:** (target_position - caster_position).normalized()

## Console Output

**Successful skillshot sequence:**
```
[InputManager] Ability slot 1 waiting for position target - click to cast
[InputManager] Cast ability at target
[AbilityComponent] Began casting ability slot 1
[SkillshotProjectile] Setup: damage=40, speed=25, max_distance=20, aoe_radius=5
[SkillshotProjectile] Hit unit: Enemy Unit
[SkillshotProjectile] Detonating at (10.5, 3.2) with radius 5.0
[SkillshotProjectile] Hit Enemy Unit for 40 damage
[SkillshotProjectile] Total hits: 1
[AbilityComponent] Executed ability slot 1
```

## Performance Considerations

### Collision Detection
- Runs every frame during projectile flight
- Traverses node tree to find units
- Early exit on first unit hit (projectile detonates immediately)
- Expected: negligible impact (few units to check)

### AoE Damage Resolution
- Occurs once per projectile detonation
- Traverses node tree and checks distances
- Should be fast with reasonable unit count (< 100 units)

## Customization

To create different skillshot types:

### Fast, Short-Range Skillshot
```
skillshot_speed = 40.0
skillshot_max_distance = 12.0
skillshot_hit_radius = 0.5
base_damage = 30.0
```

### Slow, Long-Range Skillshot
```
skillshot_speed = 15.0
skillshot_max_distance = 30.0
skillshot_hit_radius = 1.0
base_damage = 60.0
```

### Explosive with Large AoE
```
aoe_radius = 8.0
skillshot_speed = 25.0
skillshot_max_distance = 20.0
```

## Known Limitations

1. **Linear projectiles only** - No curved/arc trajectories
2. **Single unit collision** - Detonates on first unit hit (no pierce)
3. **No visual indicator** - Doesn't show targeting line/circle while aiming
4. **No ground collision** - Projectiles phase through terrain
5. **Simple hitbox** - Uses sphere-based collision detection

## Future Enhancements

1. **Piercing projectiles** - Continue past first hit, hit multiple units
2. **Bouncing projectiles** - Bounce off walls/terrain
3. **Curved trajectories** - Arc/parabolic paths
4. **Visual feedback** - Targeting line while aiming
5. **Knockback** - Push units on hit
6. **Status effects** - Apply debuffs on detonation
7. **Multiple detonation** - Chain detonations across units

## Files Modified

### New Files
- `src/components/combat/skillshot_projectile.hpp/cpp`
- `src/components/abilities/effects/skillshot_effect.hpp/cpp`
- `GodotGame/resources/abilities/TestSkillshotEffect.tres`

### Modified Files
- `src/components/abilities/ability_types.hpp` - Added SKILLSHOT enum
- `src/components/abilities/ability_definition.hpp/cpp` - Added skillshot properties
- `src/components/abilities/effects/CMakeLists.txt` - Added skillshot_effect
- `src/components/combat/CMakeLists.txt` - Added skillshot_projectile
- `src/register_types.cpp` - Registered SkillshotEffect, SkillshotProjectile
- `GodotGame/resources/abilities/W_Fireball.tres` - Updated to SKILLSHOT type

## Testing Checklist

- [ ] Press W key - should enter targeting mode
- [ ] Click near character - projectile should spawn and travel
- [ ] Hit enemy with projectile - should detonate immediately
- [ ] Click far away - projectile should travel full distance then detonate
- [ ] Multiple enemies in AoE - all should take damage
- [ ] Cooldown works - can't cast again before 8 seconds
- [ ] Mana cost deducted - mana goes down by 50 on cast
- [ ] Console shows all expected messages
- [ ] Projectile path looks reasonable
- [ ] Damage numbers match expected values (40 damage base)
