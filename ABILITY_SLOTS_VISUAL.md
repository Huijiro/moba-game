# Ability Slots: Visual Reference

## Quick Overview

Every unit has **4 ability slots** that map directly to keyboard keys and the ability system:

```
┌─────────────────────────────────────────────────────────────┐
│                    UNIT ABILITIES                           │
├──────────┬──────────┬──────────┬──────────┬──────────────────┤
│  SLOT 0  │  SLOT 1  │  SLOT 2  │  SLOT 3  │  In Code         │
├──────────┼──────────┼──────────┼──────────┼──────────────────┤
│   Q KEY  │  W KEY   │  E KEY   │  R KEY   │  array[0..3]     │
├──────────┼──────────┼──────────┼──────────┼──────────────────┤
│ Ability  │ Ability  │ Ability  │ Ability  │ AbilityDefinition│
│    1     │    2     │    3     │    4     │  References      │
└──────────┴──────────┴──────────┴──────────┴──────────────────┘
```

## Current Test Setup

```
TestHero.tres
├─ Slot 0 (Q) → Q_Instant_Strike.tres
│   ├─ Name: "Quick Strike"
│   ├─ Type: INSTANT (no delay)
│   ├─ Target: Single Unit
│   ├─ Damage: 25
│   ├─ Mana: 20
│   ├─ Cooldown: 5s
│   └─ Effect: TestDamageEffect
│
├─ Slot 1 (W) → W_Fireball.tres
│   ├─ Name: "Fireball"
│   ├─ Type: CAST_TIME (1 second)
│   ├─ Target: Point on ground
│   ├─ Damage: 40
│   ├─ Mana: 20
│   ├─ Cooldown: 8s
│   └─ Effect: TestDamageEffect
│
├─ Slot 2 (E) → E_Beam.tres
│   ├─ Name: "Energy Beam"
│   ├─ Type: CHANNEL (hold 2 seconds)
│   ├─ Target: Single Unit
│   ├─ Damage: 50
│   ├─ Mana: 20
│   ├─ Cooldown: 6s
│   └─ Effect: TestDamageEffect
│
└─ Slot 3 (R) → R_Explosion.tres
    ├─ Name: "Explosion"
    ├─ Type: INSTANT (no delay)
    ├─ Target: Area (8m radius)
    ├─ Damage: 35
    ├─ Mana: 20
    ├─ Cooldown: 10s
    └─ Effect: TestAoEEffect
```

## How to Edit Slot Assignments

### In Godot Editor (Recommended)

**Method 1: Edit TestHero.tres Directly**

```
File Browser
├─ GodotGame/resources/units/TestHero.tres
   └─ Double-click to open in Inspector

Inspector
├─ Abilities (Array, size=4)
│  ├─ [0]: Quick Strike (Q_Instant_Strike.tres)
│  ├─ [1]: Fireball (W_Fireball.tres)
│  ├─ [2]: Energy Beam (E_Beam.tres)
│  └─ [3]: Explosion (R_Explosion.tres)
```

**Method 2: View Through Unit Node**

```
Scene Tree
├─ Unit
│  └─ Click Unit node

Inspector
├─ Unit Definition: TestHero.tres
├─ Abilities (expanded)
│  ├─ [0]: Q_Instant_Strike.tres
│  ├─ [1]: W_Fireball.tres
│  ├─ [2]: E_Beam.tres
│  └─ [3]: R_Explosion.tres
```

### Swap Example: Put Fireball on Q

**Before:**
```
Slot 0 (Q): Q_Instant_Strike.tres  ← Quick Strike
Slot 1 (W): W_Fireball.tres        ← Fireball
```

**Edit TestHero.tres:**
```
Click [0] → Select W_Fireball.tres
Click [1] → Select Q_Instant_Strike.tres
Save
```

**After:**
```
Slot 0 (Q): W_Fireball.tres        ← Fireball (now on Q!)
Slot 1 (W): Q_Instant_Strike.tres  ← Quick Strike
```

**In Game:**
```
Press Q → Casts Fireball (with 1 second cast time)
Press W → Casts Quick Strike (instant)
```

## Data Flow: Press Q

```
Player presses Q
    ↓
InputManager._input() detects ui_ability_q action
    ↓
keybind_map["ui_ability_q"] = 0  (slot 0)
    ↓
AbilityComponent.try_cast(0, target)
    ↓
Get ability_slots[0] = Quick Strike (AbilityDefinition)
    ↓
Check cooldown & mana
    ↓
Call QuickStrike.effect.execute()
    ↓
DamageEffect deals 25 damage
    ↓
Deduct 20 mana
    ↓
Start 5 second cooldown on slot 0
```

## Three Files Involved

### File 1: Effect Resource

```
TestDamageEffect.tres
├─ Type: DamageEffect (Resource)
├─ damage_amount: 25.0
└─ When executed: "Apply 25 damage to target"
```

### File 2: Ability Definition

```
Q_Instant_Strike.tres
├─ Type: AbilityDefinition (Resource)
├─ ability_name: "Quick Strike"
├─ cast_type: 0 (INSTANT)
├─ targeting_type: 0 (UNIT_TARGET)
├─ mana_cost: 20
├─ cooldown: 5.0
├─ base_damage: 25
└─ effect: TestDamageEffect.tres  ← Links to effect file
```

### File 3: Unit Definition

```
TestHero.tres
├─ Type: UnitDefinition (Resource)
├─ unit_name: "TestHero"
├─ abilities: [
│  ├─ Q_Instant_Strike.tres,   ← Slot 0 (Q)
│  ├─ W_Fireball.tres,         ← Slot 1 (W)
│  ├─ E_Beam.tres,             ← Slot 2 (E)
│  └─ R_Explosion.tres         ← Slot 3 (R)
└─ ]
```

## In the Scene

```
GodotGame/main.tscn
├─ Node3D (root)
│  ├─ Unit (CharacterBody3D)
│  │  ├─ unit_definition: TestHero.tres  ← Set in Inspector
│  │  └─ AbilityComponent (child)
│  │     ├─ ability_slots[0]: Quick Strike
│  │     ├─ ability_slots[1]: Fireball
│  │     ├─ ability_slots[2]: Energy Beam
│  │     └─ ability_slots[3]: Explosion
│  │
│  ├─ InputManager (listens for Q/W/E/R)
│  └─ ...other nodes...
```

## Slot Index Reference

Use these indices when working with code:

| Index | Key | Property | Code |
|-------|-----|----------|------|
| 0 | Q | Slot 0 | `ability_slots[0]` |
| 1 | W | Slot 1 | `ability_slots[1]` |
| 2 | E | Slot 2 | `ability_slots[2]` |
| 3 | R | Slot 3 | `ability_slots[3]` |

Example C++ code:
```cpp
AbilityComponent* ac = unit->get_ability_component();
ac->try_cast(0, target);   // Cast Q ability on target
ac->try_cast(1, nullptr);  // Cast W ability
ac->try_cast(2, target);   // Cast E ability on target
ac->try_cast(3, nullptr);  // Cast R ability
```

## Ability Types & Casting

### Slot 0 (Q): INSTANT
```
Press Q
  ↓
Ability executes immediately
  ↓
Damage dealt instantly
  ↓
Mana deducted
  ↓
Cooldown starts
```

### Slot 1 (W): CAST_TIME
```
Press W
  ↓
Show casting state (1 second delay)
  ↓
At 0.4s mark: ability fires (cast_point = 0.4)
  ↓
Damage dealt at 0.4 seconds
  ↓
Mana deducted at 0.4 seconds
  ↓
Cooldown starts at 0.4 seconds
```

### Slot 2 (E): CHANNEL
```
Press E
  ↓
Show channeling state (2 second channel)
  ↓
Player must hold (can't move)
  ↓
After 2 seconds: ability fires
  ↓
Damage dealt
  ↓
Mana deducted
  ↓
Cooldown starts
```

### Slot 3 (R): INSTANT + AREA
```
Press R
  ↓
Ability executes immediately
  ↓
Find all units in 8m radius (excluding caster)
  ↓
Deal 35 damage to each
  ↓
Mana deducted (once, shared cost)
  ↓
Cooldown starts
```

## Files to Know

| File | Purpose | Edit For |
|------|---------|----------|
| `TestHero.tres` | Main hero config | Change which abilities are on Q/W/E/R |
| `Q_Instant_Strike.tres` | Q ability config | Change Q's damage, cooldown, mana cost |
| `W_Fireball.tres` | W ability config | Change W's damage, cooldown, mana cost |
| `E_Beam.tres` | E ability config | Change E's damage, cooldown, mana cost |
| `R_Explosion.tres` | R ability config | Change R's damage, cooldown, mana cost |
| `TestDamageEffect.tres` | Single-target effect | Change damage amount |
| `TestAoEEffect.tres` | Area effect | Change area damage |

All in: `GodotGame/resources/`

## Quick Changes

### Change Q ability to be Fireball instead:
1. Open `TestHero.tres`
2. Change `[0]` from `Q_Instant_Strike` to `W_Fireball`
3. Save → Play → Q now casts Fireball

### Change Q damage from 25 to 100:
1. Open `Q_Instant_Strike.tres`
2. Change `base_damage: 25` to `base_damage: 100`
3. Save → Play → Q deals 100 damage

### Change Q cooldown from 5s to 2s:
1. Open `Q_Instant_Strike.tres`
2. Change `cooldown: 5.0` to `cooldown: 2.0`
3. Save → Play → Q cooldown is now 2 seconds

## Testing

**Test Current Setup:**
```
1. Open GodotGame/main.tscn
2. Set Unit→unit_definition to TestHero.tres
3. Press F5 to play
4. Press Q → Quick Strike (instant)
5. Press W → Fireball (cast 1 sec)
6. Press E → Energy Beam (channel 2 sec)
7. Press R → Explosion (instant, area)
8. Console shows: "[Unit] Loaded ability slot 0: Quick Strike" etc.
```

**Test Modified Setup:**
```
1. Open TestHero.tres
2. Swap [0] and [1] (Q ↔ W)
3. Save
4. Play
5. Press Q → Now casts Fireball (not Quick Strike)
6. Press W → Now casts Quick Strike (not Fireball)
```

---

**Status:** ✅ Complete
**Next Step:** ABILITY_ASSIGNMENT.md for detailed guide
