# How to Assign Abilities to Characters

This guide explains how abilities are organized and assigned to units using the ability system.

## Overview: The 3-Tier System

The ability system has 3 levels of configuration:

```
Tier 1: AbilityEffect (What it does)
   ↓
Tier 2: AbilityDefinition (How it works - contains an effect)
   ↓
Tier 3: UnitDefinition (Which abilities - contains 4 ability definitions)
   ↓
Unit (Character - loads abilities from UnitDefinition)
```

## Ability Slots: Q, W, E, R

Units have exactly **4 ability slots** that map to keyboard keys:

| Slot | Key | Index |
|------|-----|-------|
| 0 | Q | Index 0 |
| 1 | W | Index 1 |
| 2 | E | Index 2 |
| 3 | R | Index 3 |

When you press Q, it casts the ability in **slot 0**.
When you press W, it casts the ability in **slot 1**.
And so on.

## How It Works: Complete Flow

### 1. Create or Use an AbilityEffect

The effect defines **what happens** when the ability executes.

Examples:
- `TestDamageEffect.tres` - Deals 25 damage
- `TestAoEEffect.tres` - Deals 35 damage in an area

These are Resource files in `GodotGame/resources/abilities/`

### 2. Create an AbilityDefinition

The ability definition **wraps the effect** with:
- Cast type (INSTANT, CAST_TIME, CHANNEL)
- Targeting type (UNIT_TARGET, POINT_TARGET, AREA, SELF_CAST)
- Mana cost
- Cooldown
- Range
- AoE radius
- The effect to execute

Example: `Q_Instant_Strike.tres`
```
ability_name = "Quick Strike"
cast_type = 0 (INSTANT)
targeting_type = 0 (UNIT_TARGET)
mana_cost = 20
cooldown = 5
base_damage = 25
effect = TestDamageEffect.tres
```

### 3. Create a UnitDefinition

The unit definition **organizes abilities** into 4 slots (Q/W/E/R).

Example: `TestHero.tres`
```
unit_name = "TestHero"
unit_id = 1
description = "Test hero with all 4 ability types"
unit_type = "hero"
abilities = [
  Q_Instant_Strike.tres,      ← Slot 0 (Q key)
  W_Fireball.tres,            ← Slot 1 (W key)
  E_Beam.tres,                ← Slot 2 (E key)
  R_Explosion.tres            ← Slot 3 (R key)
]
```

### 4. Set UnitDefinition on Unit

In the Godot editor:
1. Select the **Unit** node in scene tree
2. In Inspector, find **Unit Definition** property
3. Drag `TestHero.tres` to that field

When the scene plays:
- Unit loads TestHero definition
- All 4 abilities are automatically populated to AbilityComponent
- Pressing Q/W/E/R casts abilities from slots 0/1/2/3

## Editing in Godot Editor

### Option 1: Edit TestHero.tres File Directly

1. In File Browser, open `res://resources/units/TestHero.tres`
2. Double-click to open in inspector
3. Look for **Abilities** property (shows as array)
4. Click [0], [1], [2], [3] to see/edit each ability

Result:
```
Abilities
  [0] Q_Instant_Strike.tres    ← Slot 0 (Q)
  [1] W_Fireball.tres          ← Slot 1 (W)
  [2] E_Beam.tres              ← Slot 2 (E)
  [3] R_Explosion.tres         ← Slot 3 (R)
```

### Option 2: Edit in Scene Inspector

1. In main scene, select **Unit** node
2. In Inspector, look for **Unit Definition** = TestHero.tres
3. With TestHero selected, scroll down to see **Abilities** array
4. Click each [0], [1], [2], [3] to see what abilities are assigned

**Note:** The abilities are shown as read-only references when viewing through the Unit. To edit them, open TestHero.tres directly.

## Visual Representation

```
GodotGame/resources/abilities/

TestDamageEffect.tres ─┐
                      ├─→ Q_Instant_Strike.tres ─┐
                      │                          │
TestAoEEffect.tres ───┼─→ W_Fireball.tres ──────┤
                      │                          ├─→ TestHero.tres ─→ Unit
Frost_Bolt_Effect ────┼─→ E_Beam.tres ───────────┤     (4 slots)
                      │                          │
                      └─→ R_Explosion.tres ──────┘

Ability Slots:
  Slot 0 (Q key) → Q_Instant_Strike.tres
  Slot 1 (W key) → W_Fireball.tres
  Slot 2 (E key) → E_Beam.tres
  Slot 3 (R key) → R_Explosion.tres
```

## Current Setup: TestHero

TestHero.tres is pre-configured with:

| Slot | Key | Ability | Type | Target | Damage | Cooldown |
|------|-----|---------|------|--------|--------|----------|
| 0 | Q | Quick Strike | INSTANT | Unit | 25 | 5.0s |
| 1 | W | Fireball | CAST_TIME | Point | 40 | 8.0s |
| 2 | E | Energy Beam | CHANNEL | Unit | 50 | 6.0s |
| 3 | R | Explosion | INSTANT | Area | 35 | 10.0s |

All abilities cost 20 mana.

## How to Create Your Own Hero

### Step 1: Create New Unit Definition
1. Right-click in `res://resources/units/`
2. Select "New Resource" → "UnitDefinition"
3. Name it `MyHero.tres`
4. Edit properties:
   - unit_name: "My Hero"
   - unit_id: 2 (or next available)
   - unit_type: "hero"

### Step 2: Assign Abilities
1. In Abilities array, set size to 4
2. Click [0] → Resource picker
3. Select ability from `res://resources/abilities/`
4. Repeat for [1], [2], [3]

### Step 3: Use in Scene
1. Open `main.tscn`
2. Select Unit node
3. Set Unit Definition to MyHero.tres
4. Play and test

## Example: Swap Abilities

**Current Setup (TestHero):**
- Slot 0: Quick Strike
- Slot 1: Fireball
- Slot 2: Energy Beam
- Slot 3: Explosion

**To Swap Q and W:**
1. Open `TestHero.tres`
2. In Abilities array:
   - Change [0] from Quick Strike → Fireball
   - Change [1] from Fireball → Quick Strike
3. Save
4. Play - Q now casts Fireball, W casts Quick Strike

## File Organization

```
GodotGame/resources/
│
├─ abilities/
│  ├─ TestDamageEffect.tres         ← Effect (25 dmg)
│  ├─ TestAoEEffect.tres            ← Effect (35 dmg AOE)
│  ├─ Q_Instant_Strike.tres         ← Ability
│  ├─ W_Fireball.tres               ← Ability
│  ├─ E_Beam.tres                   ← Ability
│  └─ R_Explosion.tres              ← Ability
│
└─ units/
   ├─ TestHero.tres                 ← Hero (has 4 abilities)
   └─ MageHero.tres                 ← Hero (variant with Frost Bolt)
```

## Code Behind the Scenes

### When Unit Loads (in Unit._ready())

```cpp
if (unit_definition.is_valid() && ability_component != nullptr) {
    for (int i = 0; i < 4; i++) {
        auto ability = unit_definition->get_ability(i);
        if (ability.is_valid()) {
            ability_component->set_ability(i, ability);
            // Prints: "[Unit] Loaded ability slot 0: Quick Strike"
        }
    }
}
```

### When Q is Pressed (in InputManager._input())

```cpp
if (event->is_action_pressed("ui_ability_q")) {
    _handle_ability_input("ui_ability_q");
}

// Looks up keybind map:
// keybind_map["ui_ability_q"] = 0  ← Ability slot 0

// Calls:
ability_component->try_cast(0, target);  ← Cast slot 0 ability
```

## Debugging

### Check What Abilities Are Loaded

1. Play the scene
2. Open Godot console (View → Toggle Output Console)
3. You should see:
   ```
   [Unit] Loaded ability slot 0: Quick Strike
   [Unit] Loaded ability slot 1: Fireball
   [Unit] Loaded ability slot 2: Energy Beam
   [Unit] Loaded ability slot 3: Explosion
   ```

### Verify Ability is in Slot

1. Select Unit in scene tree
2. Select AbilityComponent child
3. In Inspector, look for individual ability slots
4. Should show ability name and reference

### Test Casting

1. Press Q/W/E/R keys
2. Console should show:
   ```
   [AbilityComponent] Attempting to cast ability: Quick Strike
   ```
3. If ability doesn't cast:
   - Check cooldown (message shows if on cooldown)
   - Check mana (ResourcePoolComponent.current_mana)
   - Check targeting (for point-target, need to set target position)

## Common Issues

### "Slot 0 is empty / no ability to cast"
- Check Unit Definition is set on Unit node
- Check UnitDefinition.abilities array has 4 items
- Check each slot has a valid AbilityDefinition resource

### "Ability doesn't execute"
- Check Unit Definition is loaded (console message in _ready)
- Press Q/W/E/R - should see "[AbilityComponent] Attempting to cast"
- If no message: InputManager not connected or no AbilityComponent

### "Wrong ability casts on Q"
- Check keybind map (Q should be slot 0)
- Check abilities array in UnitDefinition
- Check you edited the right file

### "Need different Q ability"
- Open UnitDefinition.tres
- Click slot [0] in Abilities array
- Select different AbilityDefinition resource
- Save

## Next Steps

1. **Test Current Setup:**
   - Play with TestHero
   - Press Q/W/E/R and verify abilities cast

2. **Modify TestHero:**
   - Open TestHero.tres
   - Swap abilities around
   - See abilities execute on different keys

3. **Create Custom Hero:**
   - Follow "How to Create Your Own Hero" section above
   - Use existing abilities (Q_Instant_Strike, W_Fireball, etc.)
   - Assign to new UnitDefinition

4. **Create Custom Ability:**
   - See docs/ability-system/01_ADDING_ABILITIES.md
   - Create new AbilityDefinition resource
   - Create new AbilityEffect resource (or use existing)
   - Assign to UnitDefinition

---

## Summary

**Ability Assignment Flow:**

```
Create AbilityEffect
    ↓
Create AbilityDefinition (wraps effect + parameters)
    ↓
Assign to UnitDefinition slot 0/1/2/3 (Q/W/E/R)
    ↓
Set UnitDefinition on Unit node
    ↓
Unit loads abilities in _ready()
    ↓
Press Q/W/E/R to cast abilities
```

**Current Test Setup:**
- TestHero.tres has all 4 slots filled
- Slot 0 (Q) = Quick Strike
- Slot 1 (W) = Fireball
- Slot 2 (E) = Energy Beam
- Slot 3 (R) = Explosion

**To Change Abilities:**
- Edit the .tres file
- Change which ability resource is in each slot [0] [1] [2] [3]
- Save and play

---

**Last Updated:** February 1, 2026
**Status:** ✅ Complete & Tested
