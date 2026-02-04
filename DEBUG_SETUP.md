# Debug System Setup Guide

## Quick Setup

### 1. Build the Project

The DebugLogger is built into the C++ GDExtension. Just rebuild:

```bash
cmake -S . -DCMAKE_BUILD_TYPE=Debug
ninja
```

The logger is ready to use immediately - no additional setup needed.

### 2. Create Visual Debugger in Your Scene (Optional)

If you want visual debugging in the viewport, add to your main scene:

```gdscript
func _ready():
  var debugger = VisualDebugger.new()
  add_child(debugger)
  debugger.set_debug_enabled(true)
```

### 3. Start Using Debug Logging

In your C++ code, use the logging macros - they work immediately:

```cpp
#include "debug/debug_macros.hpp"

// Simple logging - works right away, no setup needed
DBG_INFO("Movement", "Unit started moving");
DBG_WARN("Combat", "Target out of range");
DBG_ERROR("Physics", "Collision failed");

// With values
DBG_VALUE("Speed", "current_speed", speed);
DBG_VECTOR3("Position", "pos", unit->get_global_position());
```

The logger is **lazy-initialized** - it creates and registers itself on the first `DBG_*` macro call.

## Verification

To verify the logger is working:

1. **First Log Message:**
   - When you make your first `DBG_*` call, the logger creates and registers itself
   - You should see: `[DebugLogger] Initialized with log level: DEBUG`

2. **In Game Output:**
   - Run the game and check the Godot Output panel
   - You should see your `DBG_*` messages with `[INFO]`, `[DEBUG]`, etc. prefixes

3. **Visual Debugging:**
   - Press **D** to toggle visual debugging on/off
   - Visual markers should appear in the viewport

## Troubleshooting

### Logs Not Appearing

1. **Check Godot Output panel** (View > Output)
2. **Verify the library is loaded:**
   - Open main.tscn in the editor
   - Check that C++ classes are available
3. **Try calling a DBG macro** - the logger initializes on first use:
   ```cpp
   DBG_INFO("Test", "Hello from C++");
   ```
4. **Check log level:**
   ```cpp
   auto* logger = DebugLogger::get_singleton();
   if (logger) {
     logger->set_log_level(::LogLevel::DEBUG);
   }
   ```

### Visual Debugger Not Showing

1. Check if you created the VisualDebugger in your scene
2. Make sure you called `debugger.set_debug_enabled(true)`
3. Try pressing **D** to toggle visual debugging

## Files Involved

- `src/debug/debug_logger.hpp/.cpp` - Logger class (C++)
- `src/debug/debug_macros.hpp` - Logging macros (C++)
- `src/debug/visual_debugger.hpp/.cpp` - Visual debugging (C++)
- `.opencode/skills/moba-debugging/SKILL.md` - Complete documentation

## Next Steps

1. Load the debugging skill: `/skill moba-debugging`
2. Review API reference and examples
3. Add logging to your components
4. Use visual debugging for spatial systems

## See Also

- `.opencode/skills/moba-debugging/SKILL.md` - Complete API reference
- `AGENTS.md` - Development workflow overview
