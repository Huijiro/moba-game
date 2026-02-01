# Quick Start: Setting Up Abilities for Testing

This guide walks you through setting up a character with abilities in the Godot editor so you can test the ability system immediately.

## Prerequisites

✅ Rebuild the library after the Resource fix:
```bash
cd /home/huijiro/Dev/rpg-game
ninja
```

✅ Restart Godot completely (close and reopen)
- This forces Godot to load the updated library

## Quick Setup (5 minutes)

### Step 1: Open the Scene

1. In Godot editor, open `GodotGame/main.tscn`
2. In the Scene tree on the left, find the **Unit** node
3. Click on it to select it

### Step 2: Set Unit Definition

1. Look at the **Inspector** panel on the right
2. Find the **Unit Definition** property (should be empty)
3. Click on the folder icon next to "UnitDefinition"
4. Navigate to: `res://resources/units/`
5. Select **TestHero.tres**
6. Click "Open"

**Result:** The Unit Definition is now set to TestHero, which has 4 abilities:
- Q: Quick Strike (INSTANT, single-target, 25 dmg)
- W: Fireball (CAST_TIME, point-target, 40 dmg)
- E: Energy Beam (CHANNEL, single-target, 50 dmg)
- R: Explosion (INSTANT, area, 35 dmg)

### Step 3: Verify Setup

1. In Scene tree, expand the **Unit** node
2. Look for the **AbilityComponent** child node
3. Click on it
4. In the Inspector, you should see the ability slots populated with the 4 abilities

✅ **Abilities are now configured!**

## Test It!

1. **Run the scene:** Press F5 or click the Play button in top-right
2. **In-game controls:**
   - **Mouse Click:** Move the character
   - **Q Key:** Cast Quick Strike (must have target selected or nearby)
   - **W Key:** Cast Fireball (clicks to place)
   - **E Key:** Cast Energy Beam (must have target)
   - **R Key:** Cast Explosion (area around character)

## What Should Happen

| Action | Expected Result |
|--------|-----------------|
| Click on ground | Character moves there |
| Press Q | Shows casting state, deals damage to nearby unit (if any) |
| Press W | Shows casting time (1 second), then casts at your location |
| Press E | Channels for 2 seconds, then deals damage |
| Press R | Instantly creates explosion around character |
| Check Console | Should see ability names being loaded and cast |

## Troubleshooting

### "Unit Definition" property not showing in Inspector
- Make sure you've restarted Godot completely after rebuilding
- Check that the library built successfully: `ls -lh GodotGame/lib/Linux-x86_64/libGodotGame-d.so`
- Try restarting Godot again

### Abilities don't appear in AbilityComponent
- Open TestHero.tres to verify it has 4 abilities
- Check that Unit Definition is set on the Unit node
- Play the scene - abilities should load in `_ready()`
- Check console output for "[Unit] Loaded ability slot" messages

### Q/W/E/R keys don't work
- Check that InputManager is in the scene tree
- Verify the InputManager has a camera reference
- Check Godot console for input debug messages

### Character doesn't move
- Click on the ground in the game viewport
- Make sure the click indicator appears (small visual marker)
- Character should walk toward the clicked location

### Mana doesn't deduct
- Open the Unit node
- Check ResourcePoolComponent has max_mana = 200.0
- Abilities in TestHero should have mana_cost > 0
- Check console for ability casting messages

## Next Steps

Once you have abilities working:

1. **Test each ability type:**
   - Q: INSTANT (no delay)
   - W: CAST_TIME (1 second delay)
   - E: CHANNEL (2 second channel)
   - R: AREA (affects all units in radius)

2. **Experiment with editor:**
   - Open TestHero.tres to see ability configuration
   - Open Q_Instant_Strike.tres to see ability definition
   - Open TestDamageEffect.tres to see effect configuration

3. **Create custom abilities:**
   - See docs/ability-system/01_ADDING_ABILITIES.md

## File Locations

| Resource | Path |
|----------|------|
| Test Hero | `GodotGame/resources/units/TestHero.tres` |
| Q Ability | `GodotGame/resources/abilities/Q_Instant_Strike.tres` |
| W Ability | `GodotGame/resources/abilities/W_Fireball.tres` |
| E Ability | `GodotGame/resources/abilities/E_Beam.tres` |
| R Ability | `GodotGame/resources/abilities/R_Explosion.tres` |
| Main Scene | `GodotGame/main.tscn` |
| Player Unit | `GodotGame/unit.tscn` |

## System Architecture

Here's how it all works together:

```
Unit (Character)
├── unit_definition = TestHero.tres
│   └── Contains 4 abilities (Q/W/E/R)
│
├── AbilityComponent (Manages abilities)
│   ├── Slot 0 → Q_Instant_Strike
│   ├── Slot 1 → W_Fireball
│   ├── Slot 2 → E_Beam
│   └── Slot 3 → R_Explosion
│
├── ResourcePoolComponent (Mana)
│   └── current_mana = 200.0
│
└── When Q/W/E/R pressed:
    1. InputManager detects key
    2. Calls AbilityComponent.try_cast()
    3. Checks cooldown, mana, cast state
    4. Executes AbilityEffect
    5. Deducts mana, starts cooldown
```

## Console Output Example

When you play and cast abilities, you should see:
```
[Unit] Loaded ability slot 0: Quick Strike
[Unit] Loaded ability slot 1: Fireball
[Unit] Loaded ability slot 2: Energy Beam
[Unit] Loaded ability slot 3: Explosion
[AbilityComponent] Attempting to cast ability: Quick Strike
[AbilityComponent] Quick Strike on cooldown for 5.0 seconds
```

---

**Status:** Ready to test!
**Next:** See docs/ability-system/03_TESTING.md for comprehensive testing guide
