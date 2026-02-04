# Modifying Unit Definitions - How To Guide

This guide shows how to create new unit definitions or modify existing ones to use different abilities.

## Quick Reference

**Unit Definition Location:** `res://resources/units/`

**Available Units:**
- `TestHero.tres` - Test hero with all 4 ability types (default)
- `MageHero.tres` - Mage specialization with Frost Bolt (Q)

## Modifying an Existing Unit Definition

### Example 1: Replace One Ability in TestHero

Let's say you created a new ability `Lightning_Strike.tres` and want to replace TestHero's Q ability (Quick Strike).

#### In Godot Editor (Visual)

1. Open File Browser, navigate to `res://resources/units/`
2. Double-click `TestHero.tres` to open in Inspector
3. In the Inspector panel, expand the `abilities` array
4. You see 4 slots:
   ```
   [0] Quick Strike
   [1] Fireball
   [2] Energy Beam
   [3] Explosion
   ```
5. Click on slot `[0]` (Quick Strike)
6. In the Inspector, you see a resource picker with "Quick Strike" selected
7. Click the **resource folder icon** next to it
8. Browse to `res://resources/abilities/Lightning_Strike.tres`
9. Click **Open** to select it
10. The Inspector updates to show "Lightning Strike"
11. Press Ctrl+S to save `TestHero.tres`

Result: TestHero now has Lightning Strike as Q, keeping W/E/R the same.

#### In .tres File (Manual)

Open `TestHero.tres` in a text editor:

**Before:**
```
[gd_resource type="UnitDefinition" format=3]
[ext_resource type="Resource" path="res://resources/abilities/Q_Instant_Strike.tres" id="1_q"]
...
abilities = [ExtResource("1_q"), ExtResource("2_w"), ExtResource("3_e"), ExtResource("4_r")]
```

**After:**
```
[gd_resource type="UnitDefinition" format=3]
[ext_resource type="Resource" path="res://resources/abilities/Lightning_Strike.tres" id="1_q"]
...
abilities = [ExtResource("1_q"), ExtResource("2_w"), ExtResource("3_e"), ExtResource("4_r")]
```

### Example 2: Create a Warrior Unit

Create a new unit definition with warrior-specific abilities.

#### Step 1: Ensure You Have Warrior Abilities

You need 4 ability definitions first. For example:
- `Slash.tres` - Quick melee strike (INSTANT, UNIT_TARGET)
- `Whirlwind.tres` - Area spin attack (INSTANT, AREA)
- `Charge.tres` - Charge to target (CAST_TIME, UNIT_TARGET)
- `Shout.tres` - AOE buff (INSTANT, AREA)

#### Step 2: Create WarriorHero.tres

**In Godot Editor:**
1. Right-click in `res://resources/units/`
2. Select **New Resource** → **UnitDefinition**
3. In Inspector, fill out:
   - `unit_name` = "Warrior"
   - `unit_id` = 3
   - `description` = "Melee warrior specializing in close combat"
   - `unit_type` = "hero"

4. Set `abilities` array:
   - Click the `abilities` property
   - Set Array size to 4
   - Slot [0]: Drag `Slash.tres` or use resource picker
   - Slot [1]: Drag `Whirlwind.tres`
   - Slot [2]: Drag `Charge.tres`
   - Slot [3]: Drag `Shout.tres`

5. Save as `WarriorHero.tres` in `res://resources/units/`

**In .tres File (Manual):**
```
[gd_resource type="UnitDefinition" format=3]
[ext_resource type="Resource" path="res://resources/abilities/Slash.tres" id="1_q"]
[ext_resource type="Resource" path="res://resources/abilities/Whirlwind.tres" id="2_w"]
[ext_resource type="Resource" path="res://resources/abilities/Charge.tres" id="3_e"]
[ext_resource type="Resource" path="res://resources/abilities/Shout.tres" id="4_r"]

[resource]
unit_name = "Warrior"
unit_id = 3
description = "Melee warrior specializing in close combat"
unit_type = "hero"
abilities = [ExtResource("1_q"), ExtResource("2_w"), ExtResource("3_e"), ExtResource("4_r")]
```

## Using a Unit Definition in a Scene

### Example: Swap TestHero for MageHero

#### In Godot Editor

1. Open your scene (`main.tscn` or `unit.tscn`)
2. In the Scene tree, select the **Unit** node
3. In the Inspector, find the `unit_definition` property
4. Click the resource picker button (folder icon)
5. Navigate to and select `MageHero.tres`
6. The property updates to show "MageHero"
7. Save the scene (Ctrl+S)

Now when the scene runs, the Unit will load MageHero's abilities instead of TestHero's.

#### In .tscn File (Manual)

Open your scene file in a text editor:

**Before:**
```
[ext_resource type="Resource" path="res://resources/units/TestHero.tres" id="1_testunit"]

[node name="Unit" type="Unit" unique_id=1290899300]
unit_definition = ExtResource("1_testunit")
```

**After:**
```
[ext_resource type="Resource" path="res://resources/units/MageHero.tres" id="1_testunit"]

[node name="Unit" type="Unit" unique_id=1290899300]
unit_definition = ExtResource("1_testunit")
```

## Creating Variants of the Same Hero

### Example: TestHero vs TestHero Advanced

Sometimes you want the same hero with different ability builds.

#### Method 1: Duplicate File

1. In File Browser, right-click `TestHero.tres`
2. Select **Duplicate**
3. A file `TestHero (copy).tres` appears
4. Rename to `TestHero_Advanced.tres`
5. Double-click to edit
6. Modify the abilities in the `abilities` array
7. Change `unit_id` to a unique number (e.g., 11)
8. Save

#### Method 2: Create from Scratch

Follow the "Create a Warrior Unit" example above, but reuse abilities from TestHero.

**Result:**
```
resources/units/
├── TestHero.tres              (original, unit_id = 1)
├── TestHero_Advanced.tres     (variant, unit_id = 11)
├── MageHero.tres              (unit_id = 2)
└── WarriorHero.tres           (unit_id = 3)
```

## Common Patterns

### Shared Abilities Across Heroes

Many abilities can be shared between different heroes:

```
MageHero:
  Q: Frost_Bolt.tres
  W: W_Fireball.tres     ← shared
  E: E_Beam.tres         ← shared
  R: R_Explosion.tres    ← shared

WarriorHero:
  Q: Slash.tres
  W: W_Fireball.tres     ← shared (warrior also uses fireball)
  E: Charge.tres
  R: Ultimate_Strike.tres
```

This is perfectly valid! The same ability file can be used by multiple heroes.

### Theming Abilities by Hero

**Warrior Theme:** Melee, physical, armor/damage focused
- Q: Slash
- W: Whirlwind
- E: Charge
- R: Berserk

**Mage Theme:** Ranged, elemental, AOE focused
- Q: Frost_Bolt
- W: Fireball
- E: Lightning_Bolt
- R: Meteor_Strike

**Rogue Theme:** Fast, precision, cooldown focused
- Q: Quick_Stab
- W: Poison_Strike
- E: Shadow_Step
- R: Assassination

## Unit Types Beyond Heroes

### Creating a Minion Definition

Minions are weaker units controlled by AI.

```
[gd_resource type="UnitDefinition" format=3]
[ext_resource type="Resource" path="res://resources/abilities/Minion_Attack.tres" id="1_attack"]

[resource]
unit_name = "Soldier Minion"
unit_id = 101
description = "Basic soldier minion"
unit_type = "minion"
abilities = [ExtResource("1_attack"), ExtResource("1_attack"), ExtResource("1_attack"), ExtResource("1_attack")]
```

Note: All 4 slots can use the same ability (minions may only use one)

### Creating a Boss Definition

Bosses have powerful, unique abilities.

```
[gd_resource type="UnitDefinition" format=3]
[ext_resource type="Resource" path="res://resources/abilities/Boss_Slam.tres" id="1_slam"]
[ext_resource type="Resource" path="res://resources/abilities/Boss_Aura.tres" id="2_aura"]
...

[resource]
unit_name = "Dragon Lord"
unit_id = 1001
description = "Epic boss encounter"
unit_type = "boss"
abilities = [ExtResource("1_slam"), ExtResource("2_aura"), ...]
```

## Testing Your Modified Unit

1. Open the scene containing your unit
2. Select the Unit node in the Scene tree
3. Check the `unit_definition` property - it should reference your new unit
4. Press F5 to run
5. Check the Output console for: `[Unit] Loaded ability slot X: YourAbility`
6. All 4 abilities should load successfully
7. Test by pressing Q/W/E/R keys

If you don't see the loading messages:
- Unit definition may not be assigned correctly
- AbilityComponent might not be a child of Unit
- Ability slots may be empty in the unit definition

## Organization Tips

**Keep it Clean:**
```
resources/
├── abilities/
│   ├── Q_*.tres          ← Q abilities
│   ├── W_*.tres          ← W abilities
│   ├── E_*.tres          ← E abilities
│   ├── R_*.tres          ← R abilities
│   └── effects/          ← all effects
└── units/
    ├── heroes/           ← hero definitions
    ├── enemies/          ← enemy definitions
    └── minions/          ← minion definitions
```

**Naming Convention:**
- Abilities: `{SlotOrName}_{AbilityName}.tres`
  - `Q_Slash.tres`, `W_Fireball.tres`, `Frost_Bolt.tres`
- Effects: `{EffectType}_{Name}.tres`
  - `DamageEffect_Basic.tres`, `AoEEffect_Large.tres`
- Units: `{UnitName}Hero.tres`
  - `TestHero.tres`, `WarriorHero.tres`, `MageHero.tres`

## Summary

1. **Create or find 4 AbilityDefinition files** (Q, W, E, R)
2. **Create a UnitDefinition file** with these 4 abilities
3. **Assign the UnitDefinition** to a Unit node in your scene
4. **Test** by running the scene and checking console output

All done in the Godot editor—no code changes needed!
