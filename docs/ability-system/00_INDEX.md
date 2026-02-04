# Ability System - Complete Documentation Index

Welcome to the MOBA Game ability system! This document serves as an entry point to all ability-related documentation and guides.

## Quick Links

### For Testing & Playing
- **[TEST_ABILITIES.md](TEST_ABILITIES.md)** - How to test the test abilities in Godot
  - Setup instructions
  - Expected behavior for each ability type
  - Testing checklist
  - Troubleshooting guide

### For Creating & Modifying Content
- **[ADDING_ABILITIES.md](ADDING_ABILITIES.md)** - How to add new abilities
  - Step-by-step ability creation
  - Effect types (DamageEffect, AoEDamageEffect)
  - Cast types (INSTANT, CAST_TIME, CHANNEL)
  - Targeting types (UNIT_TARGET, POINT_TARGET, AREA, SELF_CAST)
  - Complete Frost Bolt example
  - Common mistakes and solutions

- **[MODIFYING_UNITS.md](MODIFYING_UNITS.md)** - How to create and modify heroes
  - Modifying existing unit definitions
  - Creating new heroes from scratch
  - Using different unit definitions in scenes
  - Creating unit variants
  - Support for minions and bosses

### For Understanding the Architecture
- **[AGENTS.md](AGENTS.md)** - Overall project architecture
  - Core classes (Unit, AbilityComponent, etc.)
  - Project structure
  - Building and testing

## Ability System Overview

### Core Components

**1. AbilityEffect** (C++ Base Class)
- What the ability *does* when executed
- Examples: DamageEffect, AoEDamageEffect
- Created as `.tres` Resource files

**2. AbilityDefinition** (Godot Resource)
- How the ability *behaves* when cast
- Properties: cost, cooldown, cast time, targeting, range
- Can reference an AbilityEffect
- Created as `.tres` Resource files

**3. UnitDefinition** (Godot Resource)
- Which abilities a unit *has*
- 4 ability slots: Q, W, E, R
- Can be assigned to any unit type
- Created as `.tres` Resource files

**4. Unit** (C++ Class)
- The character entity
- Contains AbilityComponent and other systems
- Auto-loads abilities from UnitDefinition in _ready()

**5. AbilityComponent** (C++ Component)
- Manages ability slots and casting
- Handles state machine (idle, casting, channeling, cooldown)
- Validates mana costs
- Executes effects

## Test Abilities Included

| Name | Slot | Type | Targeting | Cost | CD |
|------|------|------|-----------|------|-----|
| Quick Strike | Q | INSTANT | Unit | 20m | 5s |
| Fireball | W | CAST_TIME 1s | Point | 50m | 8s |
| Energy Beam | E | CHANNEL 2s | Unit | 30m | 6s |
| Explosion | R | INSTANT | Area 8m | 80m | 12s |
| Frost Bolt | Q alt | CAST_TIME 0.7s | Unit | 40m | 5s |

## Example Units

**TestHero** (Default)
- Q: Quick Strike (instant melee)
- W: Fireball (projectile)
- E: Energy Beam (channel)
- R: Explosion (AoE)

**MageHero** (Example Variant)
- Q: Frost Bolt (frost magic)
- W: Fireball (fire magic)
- E: Energy Beam (arcane)
- R: Explosion (devastation)

## File Structure

```
src/
├── common/
│   ├── unit_definition.hpp/.cpp         ← Unit configuration data
│   └── unit_definition_manager.hpp/.cpp ← Unit registry/lookup
├── components/abilities/
│   ├── ability_component.hpp/.cpp       ← Ability casting system
│   ├── ability_definition.hpp/.cpp      ← Ability configuration
│   ├── ability_effect.hpp/.cpp          ← Base effect class
│   ├── ability_types.hpp                ← Enums
│   └── effects/
│       ├── damage_effect.hpp/.cpp       ← Single-target damage
│       └── aoe_damage_effect.hpp/.cpp   ← Multi-target damage

GodotGame/resources/
├── abilities/
│   ├── *.tres files                     ← Individual abilities
│   └── *Effect.tres files               ← Ability effects
└── units/
    └── *Hero.tres files                 ← Unit definitions
```

## Complete Workflow: Create a New Hero

### 1. Plan the Hero
- Choose a name and theme (Warrior, Mage, Rogue, etc.)
- Design 4 abilities with different cast types
- Assign unique unit_id

### 2. Create Ability Effects
- See [ADDING_ABILITIES.md](ADDING_ABILITIES.md) Step 1
- Create DamageEffect or AoEDamageEffect files
- Set damage values

### 3. Create AbilityDefinitions
- See [ADDING_ABILITIES.md](ADDING_ABILITIES.md) Step 2
- Create 4 `.tres` files for Q, W, E, R
- Configure cast type, targeting, cost, cooldown
- Assign effects

### 4. Create UnitDefinition
- See [MODIFYING_UNITS.md](MODIFYING_UNITS.md)
- Create `YourHeroName.tres`
- Assign 4 abilities to array slots
- Set unit_name, unit_id, description, unit_type

### 5. Test in Scene
- Open `main.tscn` or `unit.tscn`
- Assign your UnitDefinition to Unit node
- Run scene (F5)
- Check console for ability loading
- Test abilities with Q/W/E/R keys

## Key Features

✓ **No C++ Required** - All configuration via Godot editor
✓ **Data-Driven** - Abilities defined as `.tres` resources
✓ **Modular** - Mix and match abilities between heroes
✓ **Extensible** - Easy to add new cast types or targeting modes
✓ **Type-Safe** - Enums for cast types and targeting types
✓ **Auto-Loading** - Units auto-populate abilities from definitions
✓ **Mana System** - Integrated resource cost and validation
✓ **Cooldown Tracking** - Per-ability cooldown management
✓ **State Machine** - Handles casting, channeling, and idle states

## Common Tasks

### Add a New Ability to an Existing Hero
1. Create AbilityEffect (if needed)
2. Create AbilityDefinition
3. Open UnitDefinition in editor
4. Replace one of the 4 ability slots
5. Save and test

### Create a Hero Variant
1. Duplicate existing UnitDefinition file
2. Rename with new suffix (e.g., TestHero_Advanced)
3. Modify ability assignments
4. Change unit_id to unique value
5. Use in scene

### Test Different Ability Combinations
1. Create multiple UnitDefinition files
2. Each with different ability combinations
3. Quickly swap UnitDefinition in scene's Unit node
4. No rebuild needed—instant feedback

### Organize Abilities by Theme
```
resources/abilities/
├── warrior/
│   ├── Q_Slash.tres
│   ├── W_Whirlwind.tres
│   └── ...
├── mage/
│   ├── Q_Frost_Bolt.tres
│   ├── W_Fireball.tres
│   └── ...
└── rogue/
    ├── Q_Quick_Stab.tres
    ├── W_Poison.tres
    └── ...
```

## Ability Type Reference

### Cast Types

**INSTANT (0)**
- Executes immediately
- No delay
- Examples: Quick Strike, Explosion
- Config: cast_time=0, cast_point=0

**CAST_TIME (1)**
- Windup before execution
- Fires at cast_point (0.0-1.0 of duration)
- Example: Fireball (1s cast, fires at 0.6s)
- Config: cast_time > 0, 0 < cast_point < 1

**CHANNEL (2)**
- Player must hold button
- Executes if held for full duration
- Cancels if released early
- Example: Energy Beam (2s channel)
- Config: channel_duration > 0

### Targeting Types

**UNIT_TARGET (0)**
- Click on specific unit
- Respects range parameter
- Example: Quick Strike (5m)

**POINT_TARGET (1)**
- Click on ground
- Respects range parameter
- Example: Fireball (15m)

**AREA (2)**
- No targeting (centered on caster or point)
- Affects all units in aoe_radius
- Example: Explosion (8m radius)

**SELF_CAST (3)**
- No targeting required
- Affects only caster
- Example: Self-buff abilities

## Troubleshooting

### Abilities Won't Load
- Check Unit has AbilityComponent child
- Verify UnitDefinition is assigned to Unit
- Check console for error messages

### Mana Not Deducting
- Verify ResourcePoolComponent exists on Unit
- Check mana_cost > 0 in AbilityDefinition
- Verify current mana >= mana_cost

### Ability Never Fires
- For CAST_TIME: verify cast_time > 0 and 0 < cast_point < 1
- For CHANNEL: verify channel_duration > 0
- Check effect is assigned

### Wrong Unit Definition Loading
- Check inspector on Unit node
- Verify UnitDefinition path is correct
- Save scene after changing unit_definition

## Next Steps

1. **Read:** Start with [ADDING_ABILITIES.md](ADDING_ABILITIES.md)
2. **Try:** Create your own ability using Frost Bolt as example
3. **Test:** Run the scene and verify loading in console
4. **Modify:** Change TestHero to use your new ability
5. **Explore:** Create a new hero definition

## Support

For issues or questions:
- Check the troubleshooting section in relevant guide
- Look at existing examples (TestHero, MageHero, Frost Bolt)
- Review TEST_ABILITIES.md for testing procedures
- Check AGENTS.md for architecture details

---

**Last Updated:** 2026-02-01
**System Status:** Fully Functional ✓
**Testing:** All 4 ability types verified ✓
**Documentation:** Complete ✓
