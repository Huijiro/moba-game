# Debug System Setup Guide

## Quick Setup

### 1. Register the Logger as an Autoload

The DebugLogger must be registered with Godot's OS in an autoload to function properly.

**Steps:**
1. Open Godot Editor
2. Go to **Project > Project Settings > Autoload**
3. Add a new autoload:
   - **Path:** `res://DebugLogger.gd`
   - **Node Name:** `DebugLogger`
4. Click "Add"
5. Close Project Settings

The logger will now automatically register when you run the game.

### 2. Create Visual Debugger in Your Scene

In your main scene's `_ready()` function:

```gdscript
func _ready():
  var debugger = VisualDebugger.new()
  add_child(debugger)
  debugger.set_debug_enabled(true)
```

### 3. Start Using Debug Logging

In your C++ code:

```cpp
#include "debug/debug_macros.hpp"

// Simple logging
DBG_INFO("Movement", "Unit started moving");
DBG_WARN("Combat", "Target out of range");
DBG_ERROR("Physics", "Collision failed");

// With values
DBG_VALUE("Speed", "current_speed", speed);
DBG_VECTOR3("Position", "pos", unit->get_global_position());
```

## Verification

To verify the logger is working:

1. **In Godot Editor Output:**
   - When you run the game, you should see: `[DebugLogger] Registered with Godot's logging system`
   - This confirms the logger is registered

2. **In Game Output:**
   - Run the game and check the Godot Output panel
   - You should see your DBG_* messages

3. **Visual Debugging:**
   - Press **D** to toggle visual debugging on/off
   - Visual markers should appear in the viewport

## Troubleshooting

### "Logger not registered" Error

If you see errors about the logger not being registered:

1. Check Project Settings > Autoload
2. Verify `DebugLogger` is listed
3. Verify the path points to `res://DebugLogger.gd`
4. Restart the editor
5. Try running the game again

### Logs Not Appearing

1. Check Godot Output panel (View > Output)
2. Verify log level is set to DEBUG:
   ```gdscript
   var logger = DebugLogger.get_singleton()
   if logger:
     logger.set_log_level(DebugLogger.LogLevel.DEBUG)
   ```
3. Verify output is enabled:
   ```gdscript
   logger.set_log_to_output(true)
   ```

### Visual Debugger Not Showing

1. Check if you created the VisualDebugger in your scene
2. Make sure you called `debugger.set_debug_enabled(true)`
3. Try pressing **D** to toggle visual debugging

## Files Involved

- `DebugLogger.gd` - Autoload registration script (in project root)
- `src/debug/debug_logger.hpp/.cpp` - Logger class
- `src/debug/debug_macros.hpp` - Logging macros
- `src/debug/visual_debugger.hpp/.cpp` - Visual debugging
- `.opencode/skills/moba-debugging/SKILL.md` - Complete documentation

## Next Steps

1. Load the debugging skill: `/skill moba-debugging`
2. Review API reference and examples
3. Add logging to your components
4. Use visual debugging for spatial systems

## See Also

- `.opencode/skills/moba-debugging/SKILL.md` - Complete API reference
- `AGENTS.md` - Development workflow overview
