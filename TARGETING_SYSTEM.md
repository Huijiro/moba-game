# Ability Targeting System Explained

The ability system supports different targeting paradigms. Here's how they work.

## Targeting Types

There are 4 main targeting types defined in `ability_types.hpp`:

### 1. UNIT_TARGET (targeting_type = 0)

**What it is:** Ability targets a specific unit (enemy, ally, or self)

**How it works:**
1. You must have a unit selected/targeted
2. Right-click an enemy unit to set it as target
3. Your character walks toward it
4. Press ability key to cast on that unit

**Examples:**
- Q: Quick Strike (single target damage)
- E: Energy Beam (single target damage)

**Current Test Setup:**
- Q requires attack target
- E requires attack target
- They use the right-click target you set

**Code:**
```cpp
if (targeting_type == 0) {  // UNIT_TARGET
    Unit* target = controlled_unit->get_attack_target();
    ability_component->try_cast(ability_slot, target);
}
```

### 2. POINT_TARGET (targeting_type = 1)

**What it is:** Ability casts at a point in the world

**How it works:**
1. You press the ability key
2. Ability casts at your character's current position
3. (Future: Click to choose where to cast)

**Examples:**
- W: Fireball (currently casts at player position)
- Future ground-targeted spells

**Current Test Setup:**
- W casts at your character position
- Hits whatever is nearby

**Code:**
```cpp
else if (targeting_type == 1) {  // POINT_TARGET
    ability_component->try_cast_point(ability_slot, 
                                      controlled_unit->get_global_position());
}
```

### 3. AREA (targeting_type = 2)

**What it is:** Ability affects all units in an area (AoE)

**How it works:**
1. You press the ability key
2. Ability creates an effect at your character's position
3. All units in the radius take damage/effect

**Examples:**
- R: Explosion (damages in 8m radius around character)

**Current Test Setup:**
- R casts at your character position
- Affects all units in 8m radius

**Code:**
```cpp
else if (targeting_type == 2) {  // AREA
    ability_component->try_cast_point(ability_slot, 
                                      controlled_unit->get_global_position());
}
```

### 4. SELF_CAST (targeting_type = 3)

**What it is:** Ability affects only the caster

**How it works:**
1. You press the ability key
2. Ability casts on yourself
3. No targeting needed

**Examples:**
- Self-buffs
- Self-heals
- Teleports

**Current Test Setup:**
- Not currently used in test abilities

**Code:**
```cpp
else if (targeting_type == 3) {  // SELF_CAST
    ability_component->try_cast_point(ability_slot, 
                                      controlled_unit->get_global_position());
}
```

## Current Test Abilities

| Ability | Key | Type | Targeting | How to Use |
|---------|-----|------|-----------|-----------|
| Quick Strike | Q | INSTANT | UNIT_TARGET | Right-click enemy, press Q |
| Fireball | W | CAST_TIME | POINT_TARGET | Press W (casts at you) |
| Energy Beam | E | CHANNEL | UNIT_TARGET | Right-click enemy, press E |
| Explosion | R | INSTANT | AREA | Press R (damages around you) |

## Using UNIT_TARGET Abilities

### Setup
1. Right-click on the Enemy Unit
2. Your character walks toward it
3. Character enters range

### Cast
1. Press Q or E
2. Ability targets the right-clicked unit
3. Damage is applied

### Troubleshooting
- **"Invalid target" error?** → Right-click enemy first
- **Out of range?** → Wait for character to walk closer
- **No damage?** → Make sure enemy is in range

## Using POINT_TARGET Abilities

### Setup
1. Just press the ability key
2. No setup needed

### Cast
1. Press W
2. Ability casts at your character's position
3. If enemy is nearby, they take damage

### Future Improvement
Click on a location to cast abilities there:
```gdscript
# Future implementation:
if event is InputEventMouseButton and event.pressed:
    cast_position = get_mouse_world_position()
    ability_component.try_cast_point(slot, cast_position)
```

## Using AREA Abilities

### Setup
1. Just press the ability key
2. No setup needed

### Cast
1. Press R
2. Explosion triggers at your position
3. All enemies in 8m radius take damage

## Using SELF_CAST Abilities

### Setup
1. Just press the ability key
2. No setup needed

### Cast
1. Press ability key
2. Effect applies to you
3. (Future: buffs, heals, etc.)

## Configuring Targeting for Custom Abilities

When you create a custom ability in `resources/abilities/MyAbility.tres`:

```
ability_name: "My Ability"
targeting_type: 0              ← Change this!
```

### Set targeting_type to:
- `0` for UNIT_TARGET (needs attack target)
- `1` for POINT_TARGET (uses position-based casting)
- `2` for AREA (area of effect)
- `3` for SELF_CAST (affects caster only)

### Example: Create a point-target ability
1. Create new AbilityDefinition resource
2. Set `targeting_type = 1`
3. Set `range = 20` (how far it can reach)
4. Set `aoe_radius = 5` (area of effect)
5. Set effect and other properties
6. Save and test

When you press the key, it will cast at your character position with that range/area.

## Future: Click-Based Targeting

The system is designed to support click-based targeting. Implementation would be:

```cpp
void InputManager::_input(const Ref<InputEvent>& event) {
  auto mouse_event = Object::cast_to<InputEventMouseButton>(event.ptr());
  
  if (mouse_event && current_targeting_mode == POINT_TARGET) {
    Vector3 click_pos = raycast_to_ground(mouse_event->get_position());
    ability_component->try_cast_point(current_slot, click_pos);
    current_targeting_mode = NONE;
  }
}
```

This would allow:
1. Press W to start targeting
2. Cursor changes to targeting reticle
3. Click on ground to cast Fireball there
4. Ability executes at clicked location

## Code Reference

### Ability Definition Configuration
- File: `src/components/abilities/ability_types.hpp`
- Enum: `TargetingType`
- Values: UNIT_TARGET(0), POINT_TARGET(1), AREA(2), SKILLSHOT(3), SELF_CAST(4)

### InputManager Targeting Logic
- File: `src/input/input_manager.cpp`
- Method: `_handle_ability_input()` (lines 403-430)
- Checks: `ability->get_targeting_type()`

### Ability Component Casting
- File: `src/components/abilities/ability_component.hpp`
- Methods: `try_cast()` for unit targets
- Methods: `try_cast_point()` for position-based

## Testing Different Targeting Types

### Test UNIT_TARGET (Q, E)
1. Right-click Enemy Unit
2. Press Q → Uses unit target ✅
3. Press E → Uses unit target ✅
4. Press W → Uses position ✅
5. Press R → Uses position (area) ✅

### Test POINT_TARGET (W)
1. Press W with enemy nearby
2. Fireball casts at your position
3. Damage should hit enemy

### Test AREA (R)
1. Press R with enemy in 8m radius
2. Explosion triggers
3. All enemies in radius take damage

## Targeting Type Decision Tree

```
Choose targeting type based on what you want:

Is it a single-target ability?
├─ YES → Use UNIT_TARGET (0)
│   └─ Example: Fireball on specific enemy
└─ NO → Continue

Does it affect an area?
├─ YES → Use AREA (2)
│   └─ Example: Explosion around player
└─ NO → Continue

Does it only affect the caster?
├─ YES → Use SELF_CAST (3)
│   └─ Example: Self-buff, self-heal
└─ NO → Use POINT_TARGET (1)
    └─ Example: Ground-targeted skill
```

## Common Mistakes

### Setting targeting_type to wrong value
- **Wrong:** `targeting_type = "UNIT_TARGET"`
- **Right:** `targeting_type = 0`
- Use the numeric value, not the name

### Forgetting to set attack target for unit abilities
- **Problem:** UNIT_TARGET ability casts on null target
- **Solution:** Right-click enemy first to set attack target

### Point abilities not working
- **Problem:** W (Fireball) doesn't seem to do anything
- **Solution:** Press W while standing on or near enemy
- Future: Click to position will make this easier

### Area abilities affecting only one unit
- **Problem:** R (Explosion) only damages target, not area
- **Solution:** Check that targeting_type = 2 (not 0)
- Check that aoe_radius is set (default is 0)

## Next Steps

1. **Test current abilities:** See TEST_ABILITIES_NOW.md
2. **Create custom abilities:** See docs/ability-system/01_ADDING_ABILITIES.md
3. **Implement click targeting:** (Future enhancement)
4. **Add SKILLSHOT type:** Directional abilities (future)

---

**Status:** ✅ Targeting system working
**Unit-target abilities:** ✅ Working
**Position-target abilities:** ✅ Working (at caster position)
**Area abilities:** ✅ Working
**Next:** Click-based targeting implementation

