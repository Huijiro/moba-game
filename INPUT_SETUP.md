# Input Setup Guide: Ability Keys (Q/W/E/R)

## What Was Fixed

The InputMap actions for ability keys were missing from the project settings, causing these errors:

```
ERROR: The InputMap action "ui_ability_q" doesn't exist.
ERROR: The InputMap action "ui_ability_w" doesn't exist.
ERROR: The InputMap action "ui_ability_e" doesn't exist.
ERROR: The InputMap action "ui_ability_r" doesn't exist.
```

These actions are now configured in `GodotGame/project.godot`.

## Configured Input Actions

The following InputMap actions have been added:

| Action | Key | Keycode | Ability Slot |
|--------|-----|---------|--------------|
| `ui_ability_q` | Q | 81 | 0 (Q ability) |
| `ui_ability_w` | W | 87 | 1 (W ability) |
| `ui_ability_e` | E | 69 | 2 (E ability) |
| `ui_ability_r` | R | 82 | 3 (R ability) |

## If You Still See Errors

### Solution 1: Reload the Project (Recommended)

1. In Godot editor, go to **File → Reload Project**
2. Godot will reload `project.godot` with the new input actions
3. The errors should disappear

### Solution 2: Close and Reopen Godot

1. Close Godot completely
2. Reopen the project
3. The new input actions will load automatically

### Solution 3: Manual Verification

Check that `project.godot` contains the input section:

```ini
[input]

ui_ability_q={
"deadzone": 0.5,
"events": [Object(InputEventKey,...keycode":81,...)]
}
ui_ability_w={
"deadzone": 0.5,
"events": [Object(InputEventKey,...keycode":87,...)]
}
ui_ability_e={
"deadzone": 0.5,
"events": [Object(InputEventKey,...keycode":69,...)]
}
ui_ability_r={
"deadzone": 0.5,
"events": [Object(InputEventKey,...keycode":82,...)]
}
```

File location: `GodotGame/project.godot`

## How It Works

### Input Flow

```
Player presses Q key
    ↓
Godot detects key press
    ↓
InputMap translates to "ui_ability_q" action
    ↓
InputManager._input() checks for action
    ↓
Calls _handle_ability_input("ui_ability_q")
    ↓
Looks up ability slot (0 for Q)
    ↓
Calls AbilityComponent.try_cast(0, target)
    ↓
Ability executes (if valid)
```

### Keybind Map (in InputManager)

The InputManager maintains a keybind map:
```
keybind_map["ui_ability_q"] = 0;  // Q key → ability slot 0
keybind_map["ui_ability_w"] = 1;  // W key → ability slot 1
keybind_map["ui_ability_e"] = 2;  // E key → ability slot 2
keybind_map["ui_ability_r"] = 3;  // R key → ability slot 3
```

This allows remapping keys in the future without code changes.

## Customizing Keys

To change which key triggers an ability:

### Method 1: Editor (Recommended)

1. In Godot, go to **Project → Project Settings**
2. Go to **Input Map** tab
3. Look for `ui_ability_q`, `ui_ability_w`, etc.
4. Click the event under each action
5. Press the new key you want
6. Godot saves the change to `project.godot`

### Method 2: Edit project.godot Directly

1. Open `GodotGame/project.godot` in a text editor
2. Find the `[input]` section
3. Change the keycode values:
   - Q = 81, W = 87, E = 69, R = 82
   - Other keys: A = 65, D = 68, Space = 32, etc.
4. Save and reload the project in Godot

## Debugging Input Issues

### Check Console Output

When playing the scene, look for these messages:

```
[InputManager] Initialized default keybinds
[InputManager] Unit has no AbilityComponent
```

If you see the first message, input is working. If you see the second, the Unit doesn't have an AbilityComponent.

### Test Input in Editor

1. Open **Project Settings → Input Map**
2. Look for `ui_ability_q`, `ui_ability_w`, etc.
3. They should be visible and show the key bindings
4. If they're not visible, the project.godot file wasn't loaded correctly

### Verify InputManager is Running

1. In Scene tree, find the **InputManager** node
2. Check that it's not disabled (grayed out)
3. Check that it has a **controlled_unit** reference
4. Check that it has a **camera** reference

## Related Files

| File | Purpose |
|------|---------|
| `GodotGame/project.godot` | Project settings with input map |
| `src/input/input_manager.hpp` | InputManager class definition |
| `src/input/input_manager.cpp` | InputManager implementation (lines 375-405) |
| `QUICK_START.md` | Ability setup guide (references this input configuration) |

## Common Issues

### "Keycode unknown: 81"
- This is not actually an error, just a warning
- Keycode 81 is valid (Q key)
- Can be ignored

### Keys work in editor but not in exported game
- Godot uses physical keycodes in exported builds
- Input map actions should handle this automatically
- If issues persist, check that exported game includes all .godot files

### Some keys interfere with Godot shortcuts
- Q/W/E/R don't conflict with default Godot shortcuts
- If adding other keys, avoid: Ctrl+S (save), Ctrl+Q (quit), etc.

## Advanced: Custom Keybinds at Runtime

To support player-configurable keybinds:

1. Use `InputMap.action_add_event(action, event)` to add new events
2. Use `InputMap.action_erase_event(action, event)` to remove events
3. Save custom keybinds to a config file
4. Load on startup

See Godot documentation: [Using InputMap](https://docs.godotengine.org/en/stable/tutorials/inputs/using_inputmap.html)

---

**Status:** ✅ Input actions configured and ready
**Test:** Press Q/W/E/R in game and abilities should cast
**Next:** See QUICK_START.md for full ability system setup
