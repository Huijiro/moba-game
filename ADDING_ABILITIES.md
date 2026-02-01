# Adding New Abilities - Complete Guide

This guide explains how to create new abilities entirely in the Godot editor without touching any C++ code.

## Overview

The ability system is built on three core concepts:

1. **AbilityEffect** - What the ability *does* (e.g., damage, heal, buff)
2. **AbilityDefinition** - How the ability *behaves* (casting type, targeting, cost)
3. **UnitDefinition** - Which abilities a unit *has* (assigned to Q/W/E/R slots)

All are created as `.tres` (Godot Resource) files in the editor.

## Step-by-Step: Creating a New Ability

### Step 1: Choose or Create an Effect

Effects define what happens when the ability executes.

#### Available Effects

**DamageEffect**
- Single-target direct damage
- File: `resources/abilities/TestDamageEffect.tres`
- Properties:
  - `damage_amount` - Amount of damage dealt

**AoEDamageEffect**
- Multi-target area damage
- File: `resources/abilities/TestAoEEffect.tres`
- Properties:
  - `damage_amount` - Damage per target in radius

#### Creating a New DamageEffect

1. In Godot editor, open `res://resources/abilities/`
2. Right-click → New Resource
3. Select **DamageEffect**
4. Set `damage_amount` to desired value (e.g., 45.0)
5. Save as `YourEffect.tres` (e.g., `FrostBoltEffect.tres`)

**Example: Frost Bolt Effect**
```
[gd_resource type="DamageEffect" format=3]

[resource]
damage_amount = 35.0
```

### Step 2: Create the AbilityDefinition

The ability definition is the core configuration file.

#### In Godot Editor

1. Right-click in `res://resources/abilities/`
2. Select **New Resource** → **AbilityDefinition**
3. In Inspector, configure these properties:

| Property | Type | Example | Notes |
|----------|------|---------|-------|
| ability_name | String | "Frost Bolt" | Display name |
| description | String | "Slows enemy by 30%" | Tooltip text |
| mana_cost | Float | 35.0 | Resource cost |
| cooldown | Float | 6.0 | Seconds before recast |
| cast_type | Enum | 0=INSTANT, 1=CAST_TIME, 2=CHANNEL | Execution type |
| cast_time | Float | 0.8 | Duration of cast phase (CAST_TIME only) |
| cast_point | Float | 0.5 | When to execute (0.0-1.0 of cast_time) |
| channel_duration | Float | 2.0 | Hold duration (CHANNEL only) |
| targeting_type | Enum | 0=UNIT_TARGET, 1=POINT_TARGET, 2=AREA | What to target |
| range | Float | 12.0 | Max distance for unit/point targets |
| aoe_radius | Float | 6.0 | Radius for AREA targeting |
| base_damage | Float | 35.0 | Damage value (for reference) |
| effect | Resource | FrostBoltEffect.tres | Effect to execute |

4. Assign an effect by clicking the `effect` property and selecting your effect file
5. Save as `YourAbility.tres` (e.g., `Frost_Bolt.tres`)

#### Cast Type Reference

**INSTANT (0)**
- Executes immediately when cast
- No windup, no channel
- Example: Quick Strike, Explosion
- Config:
  - `cast_time = 0.0`
  - `cast_point = 0.0`

**CAST_TIME (1)**
- Has a windup before execution
- `cast_point` determines when in the cast to execute
- Example: Fireball (1s cast, fires at 0.6s)
- Config:
  - `cast_time = 1.0` (or any duration)
  - `cast_point = 0.6` (60% through cast)

**CHANNEL (2)**
- Player must hold button to maintain
- Executes if held for full duration
- Cancels if button released early
- Example: Energy Beam
- Config:
  - `channel_duration = 2.0`
  - Other cast_time fields ignored

#### Targeting Type Reference

**UNIT_TARGET (0)**
- Must click on a specific unit
- Respects `range` parameter
- Example: Quick Strike (5m), Energy Beam (10m)

**POINT_TARGET (1)**
- Click on ground location
- Respects `range` parameter
- Example: Fireball (15m)

**AREA (2)**
- Centered on caster or target point
- Affects all units in `aoe_radius`
- Example: Explosion (8m radius)

**SELF_CAST (3)**
- No targeting required
- Affects only the caster
- No range needed

### Step 3: Assign to Unit Definition

Once you have your ability definition, add it to a UnitDefinition.

#### Modifying an Existing UnitDefinition

1. Open `res://resources/units/TestHero.tres` in editor
2. In Inspector, expand the `abilities` array
3. You'll see 4 slots (Q, W, E, R):
   ```
   [0] Quick Strike
   [1] Fireball
   [2] Energy Beam
   [3] Explosion
   ```

4. To replace an ability:
   - Click the slot (e.g., [1] for W)
   - Click the resource picker button
   - Select your new ability file
   - Click OK

5. Save the UnitDefinition file

#### Creating a New UnitDefinition

For a different hero:

1. Right-click in `res://resources/units/`
2. Select **New Resource** → **UnitDefinition**
3. Configure in Inspector:
   - `unit_name` = "MyHero"
   - `unit_id` = 2 (unique ID)
   - `unit_type` = "hero" (or "minion", "boss", etc.)
   - `description` = "Description"
   - `abilities` = Array with 4 slots [Q, W, E, R]

4. For each slot, click and assign an AbilityDefinition

5. Save as `MyHero.tres`

#### Using the New Unit Definition in a Scene

1. Open your scene (e.g., `main.tscn` or `unit.tscn`)
2. Select the Unit node
3. In Inspector, find the `unit_definition` property
4. Click the resource picker
5. Select your new UnitDefinition file
6. Save the scene

## Complete Example: Creating Frost Bolt

### 1. Create FrostBoltEffect.tres

```
[gd_resource type="DamageEffect" format=3]

[resource]
damage_amount = 35.0
```

**How to create in editor:**
- Right-click `res://resources/abilities/`
- New Resource → DamageEffect
- Set `damage_amount = 35.0`
- Save as `FrostBoltEffect.tres`

### 2. Create Frost_Bolt.tres (Ability Definition)

```
[gd_resource type="AbilityDefinition" format=3]
[ext_resource type="Resource" path="res://resources/abilities/FrostBoltEffect.tres" id="1_effect"]

[resource]
ability_name = "Frost Bolt"
description = "Launch a frost bolt dealing 35 damage and slowing target by 30%"
mana_cost = 40.0
cooldown = 5.0
cast_type = 1
cast_time = 0.7
cast_point = 0.5
channel_duration = 0.0
targeting_type = 0
range = 10.0
aoe_radius = 0.0
base_damage = 35.0
effect = ExtResource("1_effect")
```

**How to create in editor:**
- Right-click `res://resources/abilities/`
- New Resource → AbilityDefinition
- Set properties as shown above
- Assign `effect = FrostBoltEffect.tres`
- Save as `Frost_Bolt.tres`

### 3. Add to TestHero (Optional)

To replace Fireball (W slot) with Frost Bolt:

1. Open `TestHero.tres` in editor
2. In Inspector, expand `abilities` array
3. Click slot [1] (Fireball)
4. Pick resource: `Frost_Bolt.tres`
5. Save

Now W key will cast Frost Bolt instead of Fireball!

## Advanced: Custom Effects

To create custom effects beyond DamageEffect, you need C++ code:

### Creating a HealEffect

This would require:

1. **C++ Class** (src/components/abilities/effects/heal_effect.hpp/.cpp)
   ```cpp
   class HealEffect : public AbilityEffect {
     float heal_amount = 50.0f;
     void execute(Unit* caster, Object* target) override;
   };
   ```

2. **Godot Registration** (register_types.cpp)
   ```cpp
   GDREGISTER_CLASS(HealEffect)
   ```

3. **Resource File** (.tres)
   ```
   [gd_resource type="HealEffect" format=3]
   [resource]
   heal_amount = 50.0
   ```

**For now:** Use DamageEffect for all effects; damage can be negative for healing.

## Common Mistakes & Solutions

### Ability Won't Show Up

**Problem:** Created ability but doesn't appear when assigned.

**Solution:**
- Check file path in ability definition (use resource picker, not manual typing)
- Ensure effect file exists and is valid
- Save the ability definition file after making changes

### Mana Cost Not Deducting

**Problem:** Cast ability but mana doesn't decrease.

**Solution:**
- Verify Unit has **ResourcePoolComponent** child node
- Check `mana_cost > 0` in ability definition
- Ensure current mana is sufficient (>= mana_cost)
- Check console for validation errors

### Ability Never Fires

**Problem:** Ability enters cast/channel state but never executes.

**Solution:**
- For CAST_TIME: verify `cast_time > 0` and `0.0 < cast_point < 1.0`
- For CHANNEL: verify `channel_duration > 0`
- Check that effect is properly assigned
- Verify targeting requirements are met

### Wrong Ability Assigned

**Problem:** Pressed Q but different ability cast.

**Solution:**
- Check keybind map in InputManager matches ability slots
- Verify unit_definition loads correctly (check console on startup)
- Confirm abilities in UnitDefinition array are in correct order [Q, W, E, R]

## Testing Your New Ability

### In Godot Editor

1. Open your scene with the Unit
2. Press F5 to run
3. Look for console output: `[Unit] Loaded ability slot X: YourAbility`
4. Press the corresponding key (Q/W/E/R)
5. Check for:
   - Mana deduction
   - Cooldown activation
   - Ability effect execution
   - No console errors

### Via Inspector

1. Select AbilityComponent node
2. Look at ability slots in Inspector
3. Should show all 4 abilities loaded
4. Verify mana costs and cooldowns display correctly

## Resource Organization Best Practices

```
resources/
├── abilities/
│   ├── effects/
│   │   ├── DamageEffect_Basic.tres
│   │   ├── DamageEffect_Strong.tres
│   │   └── AoEEffect_Large.tres
│   ├── by_cast_type/
│   │   ├── instant/
│   │   ├── cast_time/
│   │   └── channel/
│   └── [individual ability files]
└── units/
    ├── heroes/
    │   ├── Warrior.tres
    │   ├── Mage.tres
    │   └── Archer.tres
    ├── enemies/
    │   └── Goblin.tres
    └── minions/
        └── SoldierMinion.tres
```

## Summary

**Quick Steps:**
1. Create Effect (DamageEffect/AoEDamageEffect)
2. Create AbilityDefinition (assign effect, set properties)
3. Add to UnitDefinition (4 slots: Q/W/E/R)
4. Assign UnitDefinition to Unit in scene
5. Test!

**No C++ required!** All ability configuration happens in the Godot editor via `.tres` files.

## Next: Dynamic Ability Creation

Future enhancement: Load abilities from a JSON/YAML file for easier bulk creation and version control.
