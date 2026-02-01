# Ability System Testing Guide

## Overview
The ability system has been implemented and configured with test abilities covering all casting types and targeting paradigms.

## Test Setup

### Created Resources

#### Ability Effects
- `GodotGame/resources/abilities/TestDamageEffect.tres` - Single target damage (25 dmg)
- `GodotGame/resources/abilities/TestAoEEffect.tres` - Area damage (35 dmg)

#### Ability Definitions (All 4 Types)

| Slot | Name | Type | Cast Type | Targeting | Cost | Cooldown | Range/Radius |
|------|------|------|-----------|-----------|------|----------|--------------|
| Q | Quick Strike | INSTANT | Instant | Unit Target | 20 mana | 5s | 5m |
| W | Fireball | CAST_TIME | 1.0s cast, fires at 0.6s | Point Target | 50 mana | 8s | 15m |
| E | Energy Beam | CHANNEL | 2.0s channel | Unit Target | 30 mana | 6s | 10m |
| R | Explosion | INSTANT | Instant AoE | Area (8m radius) | 80 mana | 12s | 8m radius |

#### Unit Definition
- `GodotGame/resources/units/TestHero.tres` - TestHero with all 4 abilities configured

### Unit Setup (unit.tscn)
- **Unit node** has `unit_definition` set to TestHero
- **ResourcePoolComponent** configured with 200 max/current mana
- **AbilityComponent** auto-populated from TestHero definition

## Testing in Godot

### 1. Launch Editor
```bash
cd /home/huijiro/Dev/rpg-game/GodotGame
godot4 -e
```

### 2. Open main.tscn
- Opens the test scene with TestHero unit

### 3. Expected Behavior at Runtime

#### Console Output on Start
When the Unit enters the scene, you should see debug output:
```
[Unit] Loaded ability slot 0: Quick Strike
[Unit] Loaded ability slot 1: Fireball
[Unit] Loaded ability slot 2: Energy Beam
[Unit] Loaded ability slot 3: Explosion
```

This confirms abilities are auto-loaded from UnitDefinition.

#### Input Handling
- **Q, W, E, R keys** trigger ability casting (via InputManager keybind system)
- **Left Click** on ground to move unit

#### Ability Behaviors to Verify

##### Q - Quick Strike (Instant)
- Should execute immediately with no delay
- Check console for execution confirmation
- Verify 20 mana cost deducted
- Verify 5s cooldown starts

##### W - Fireball (Cast Time)
- Pressing W should start 1s cast time
- Ability executes at 0.6s mark (60% of cast)
- Check mana cost (50) and cooldown (8s)

##### E - Energy Beam (Channel)
- Pressing E should start 2s channel
- Holding E maintains the channel
- Releasing E cancels the channel
- On completion: 30 mana cost, 6s cooldown

##### R - Explosion (Instant AoE)
- Should execute immediately
- Highest mana cost (80) and longest cooldown (12s)
- Should damage all nearby enemies in 8m radius

### 4. Test Checklist

- [ ] All 4 abilities load on startup (check console)
- [ ] Q ability casts instantly
- [ ] W ability shows cast time delay
- [ ] E ability channels when held
- [ ] R ability executes instantly
- [ ] Mana depletes correctly for each ability
- [ ] Cooldowns prevent re-casting too soon
- [ ] Abilities respect range/radius parameters
- [ ] No errors in console output

## Troubleshooting

### Abilities Not Loading
- Check console for errors during _ready()
- Verify TestHero.tres has all 4 abilities assigned
- Ensure AbilityComponent is child of Unit

### Casting Not Working
- Verify InputManager is in scene with keybind map set up
- Check that Unit/AbilityComponent are added to scene tree
- Verify ResourcePoolComponent exists and has sufficient mana

### Mana Not Deducting
- Check ResourcePoolComponent exists on Unit
- Verify mana costs are > 0 in ability definitions
- Check console for any mana validation errors

## Next Steps

After testing, potential improvements:
1. Add visual feedback for ability cast/channel states
2. Implement projectile visual for W (Fireball)
3. Add sound effects for ability cast
4. Create more diverse ability definitions
5. Test with multiple units with different definitions
