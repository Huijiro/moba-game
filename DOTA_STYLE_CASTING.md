# DOTA-Style Click-to-Cast System

The ability system now uses the same casting paradigm as DOTA 2 and League of Legends.

## How It Works

### Three Casting Paradigms

#### 1. Self-Cast (Instant)
**Abilities:** Self-targeted spells (buffs, heals, etc.)

**How to use:**
1. Press ability key (Q/W/E/R)
2. Ability executes immediately on yourself
3. No clicking required

**Example:** Self-buff ability

#### 2. Unit-Target (Click-to-Cast)
**Abilities:** Single target spells (Q = Quick Strike, E = Energy Beam)

**How to use:**
1. Press ability key (Q or E)
2. Console shows: "waiting for unit target - click on target"
3. Cursor is in targeting mode
4. Click on enemy unit (or ally, depending on ability)
5. Ability casts on clicked unit

**Example:**
```
Press Q
  ↓
Console: "[InputManager] Ability slot 0 waiting for unit target"
  ↓
Click on Enemy Unit
  ↓
Ability executes on that unit
```

#### 3. Position-Target (Click Location)
**Abilities:** Ground-targeted spells (W = Fireball, R = Explosion)

**How to use:**
1. Press ability key (W or R)
2. Console shows: "waiting for position target - click to cast"
3. Cursor is in targeting mode
4. Click on any location on the ground
5. Ability casts at clicked location

**Example:**
```
Press W (Fireball)
  ↓
Console: "[InputManager] Ability slot 1 waiting for position target"
  ↓
Click on ground near enemy
  ↓
Fireball executes at that location
```

## Current Test Abilities

| Ability | Key | Type | Casting | How to Use |
|---------|-----|------|---------|-----------|
| Quick Strike | Q | INSTANT | UNIT_TARGET | Press Q → Click unit |
| Fireball | W | CAST_TIME | POINT_TARGET | Press W → Click location |
| Energy Beam | E | CHANNEL | UNIT_TARGET | Press E → Click unit |
| Explosion | R | INSTANT | AREA | Press R → Click location |

## Test It Now!

### Test Unit-Target Casting (Q)

1. Play scene
2. Press Q
3. Look at console output
4. See: `[InputManager] Ability slot 0 waiting for unit target - click on target`
5. Click on the Enemy Unit
6. Ability casts on that unit ✅

### Test Point-Target Casting (W)

1. Play scene
2. Press W
3. Look at console output
4. See: `[InputManager] Ability slot 1 waiting for position target - click to cast`
5. Click on ground near enemy
6. Fireball executes at that location ✅

### Test Both In Sequence

```
1. Press Q → click enemy → Quick Strike
2. Press W → click ground → Fireball
3. Press E → click enemy → Energy Beam
4. Press R → click ground → Explosion
```

All work smoothly without pre-selecting targets!

## Console Output Guide

### When You Press an Ability Key

**Self-cast ability:**
```
[InputManager] Self-cast ability slot 3
```
→ Ability executes immediately

**Unit-target ability:**
```
[InputManager] Ability slot 0 waiting for unit target - click on target
```
→ Waiting for you to click on a unit

**Position-target ability:**
```
[InputManager] Ability slot 1 waiting for position target - click to cast
```
→ Waiting for you to click on a location

### When You Click While Targeting

```
[InputManager] Cast ability at target
```
→ Ability executed at your click location/target

## Canceling Targeting Mode

**To cancel targeting mode:**
- Press the same ability key again (toggles off)
- Press a different ability key (switches to new targeting mode)
- Press ESC (future: not implemented yet, but planned)
- Click on empty space (right-click movement still works normally)

## How It Integrates With Movement

### Movement vs Abilities

**When NOT in targeting mode:**
- Right-click ground → Move
- Right-click enemy → Attack
- Left-click abilities → Enter targeting mode

**When IN targeting mode:**
- Right-click → Still works (moves/attacks)
- Left-click unit → Cast unit-target ability
- Left-click ground → Cast position ability
- Press ability again → Cancel targeting

### Example Flow

```
Normal gameplay:
├─ Right-click ground → Move ✓
├─ Right-click enemy → Attack ✓
└─ Press Q → Enter targeting mode

Targeting mode:
├─ Left-click enemy → Cast Q on enemy ✓
├─ Right-click ground → Still moves ✓
└─ Press W → Switch to targeting mode for W
```

## Configuring Custom Abilities

When creating abilities, the targeting type determines casting mode:

```
Targeting Type → Casting Behavior

0 (UNIT_TARGET)     → Press key → Click unit
1 (POINT_TARGET)    → Press key → Click location
2 (AREA)            → Press key → Click location
3 (SELF_CAST)       → Press key → Execute instantly
```

## Design Benefits

### Why This System?

1. **Standard MOBA gameplay** - Players expect this
2. **Clear feedback** - Console shows what's needed
3. **Flexible** - Supports all ability types
4. **No pre-selection required** - Click to cast immediately
5. **Movement still works** - Right-click during targeting
6. **Reversible** - Can cancel by pressing different key

### Comparison to Old System

**Old System:**
```
Right-click unit to select target
    ↓
Then press ability key
    ↓
Confusing order + ability doesn't always work
```

**New System:**
```
Press ability key
    ↓
Click target/location
    ↓
Clear, intuitive, just like DOTA 2
```

## Future Enhancements

### Planned Features
- [ ] Press ESC to cancel targeting mode
- [ ] Visual targeting reticle (shows when in targeting mode)
- [ ] Smart targeting (TAB cycles through targets)
- [ ] Quick-cast option (press key while holding modifier = auto-cast at cursor)
- [ ] Quick-attack option (right-click while holding modifier = move without attack)

### Smart Targeting Example

```
# Quick-cast (optional future feature):
Hold CTRL + Press Q → Casts at current cursor location instantly
Hold CTRL + Press W → Casts at current cursor location instantly

# This would allow:
- Experienced players to cast without click confirmation
- Skill ceiling: learn positioning to quick-cast
- Accessible: novices can still use normal click-to-cast
```

## Code Reference

### InputManager State Machine

```cpp
// When targeting mode is OFF (awaiting_target_slot == -1):
Right-click ground → Move order
Right-click enemy → Attack order
Press Q/W/E/R → Check ability type:
  ├─ SELF_CAST → Execute immediately
  └─ Other → Enter targeting mode (set awaiting_target_slot)

// When targeting mode is ON (awaiting_target_slot >= 0):
Left-click unit/ground → Execute ability, exit targeting mode
Right-click → Still works for movement/attack
Press other key → Switch to that ability's targeting mode
```

### Key Methods

- **_handle_ability_input()** - Called when Q/W/E/R pressed
  - Sets awaiting_target_slot and is_awaiting_unit_target
  - Enters targeting mode OR executes self-cast

- **_input()** - Handles all input
  - Checks if in targeting mode (awaiting_target_slot >= 0)
  - If yes, routes click to ability casting instead of normal click handling
  - If no, normal click handling (move, attack, interact)

## Testing Checklist

- [ ] Press Q → Console shows "waiting for unit target"
- [ ] Click unit while targeting → Ability casts
- [ ] Press W → Console shows "waiting for position target"
- [ ] Click ground while targeting → Ability casts
- [ ] Press different key during targeting → Switches modes
- [ ] Right-click during targeting → Still moves/attacks
- [ ] Multiple casts in sequence → Works smoothly
- [ ] No console errors → Clean execution

## Examples

### Example 1: Cast Q then W

```
Press Q
  ├─ Console: "waiting for unit target"
  └─ Click Enemy Unit
     └─ Quick Strike executes
        └─ Now in normal mode

Press W
  ├─ Console: "waiting for position target"
  └─ Click ground
     └─ Fireball executes
        └─ Now in normal mode
```

### Example 2: Cancel and Switch

```
Press Q
  ├─ Console: "waiting for unit target"
  
Press W (switch modes)
  ├─ Console: "waiting for position target"
  
Click ground
  └─ W ability executes (not Q!)
```

### Example 3: Movement During Targeting

```
Press Q
  ├─ Console: "waiting for unit target"
  
Right-click ground (while in targeting mode)
  ├─ Move order issued
  └─ Still in Q targeting mode
  
Click enemy unit
  └─ Q casts on that unit
```

## Comparison to Other Games

### DOTA 2
- ✅ Exact same system: click to cast
- ✅ Right-click still moves/attacks during targeting
- ✅ Can switch ability by pressing new key

### League of Legends
- ✅ Very similar: press key → click → cast
- Similar but slightly different (skill indicator shows range)

### Our System
- ✅ Matches DOTA 2 paradigm
- ✅ Console feedback for learning
- ✅ Extensible for future UI improvements

---

**Status:** ✅ DOTA-Style click-to-cast fully implemented
**How to test:** Press Q/W/E/R and click targets!
**Next:** Create visual feedback for targeting mode (reticle, etc.)

