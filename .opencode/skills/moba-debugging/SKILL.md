---
name: moba-debugging
description: Debug MOBA components using the logging and visual debugging systems
license: MIT
compatibility: opencode
metadata:
  audience: developers
  workflow: debugging
  project: moba-game
---

## What I Do

- Guide you through the comprehensive debug logging system
- Help you use visual debugging to understand game behavior in real-time
- Teach best practices for adding debug information to components
- Help troubleshoot issues with categorized, leveled logging
- Show how to visualize movement, combat, and ability systems

## When to Use Me

Use this skill when you need to:
- Add logging to track component behavior
- Debug a system visually in the Godot viewport
- Understand what's happening during game execution
- Track down bugs by examining categorized logs
- Monitor specific game systems (Movement, Combat, Abilities, etc.)

## Quick Start

### 1. Build the Project

The DebugLogger is built into the C++ GDExtension:

```bash
cmake -S . -DCMAKE_BUILD_TYPE=Debug
ninja
```

The logger is ready to use immediately - it's **lazy-initialized** on first use.

### 2. Add Logging to Your C++ Code

```cpp
#include "debug/debug_macros.hpp"

// Simple logging
DBG_INFO("Movement", "Unit started moving");
DBG_WARN("Combat", "Target out of range");
DBG_ERROR("Physics", "Collision failed");

// With values
DBG_VALUE("Speed", "current_speed", unit->get_speed());
DBG_VECTOR3("Position", "unit_pos", unit->get_global_position());
```

### 3. Initialize Visual Debugging in Your Scene (Optional)

```gdscript
# In your main scene script
func _ready():
  var debugger = VisualDebugger.new()
  add_child(debugger)
  debugger.set_debug_enabled(true)
```

### 4. Add Visual Debugging

```cpp
auto* debugger = VisualDebugger::get_singleton();
if (debugger && debugger->is_debug_enabled()) {
  // Draw attack range
  debugger->draw_circle_xz(unit_pos, attack_range, 
                          Color(1, 0, 0, 0.2f));
  
  // Draw movement vector
  debugger->draw_vector(unit_pos, velocity, 2.0f);
  
  // Mark position
  debugger->draw_cross(unit_pos, 0.5f, Color(1, 1, 1, 1));
}
```

### 5. Control Logging at Runtime

Press keyboard shortcuts:
- **D** - Toggle visual debugging on/off
- **L** - Cycle through log levels

## Logging API

### Log Levels

| Level | Verbosity | Use Case |
|-------|-----------|----------|
| DEBUG | Highest | Frame-by-frame diagnostic info |
| INFO | High | Significant events |
| WARNING | Medium | Unexpected but recoverable |
| ERROR | Low | Serious problems |

### Logging Macros

**Simple Messages:**
```cpp
DBG_DEBUG(category, "message")    // Verbose diagnostic
DBG_INFO(category, "message")     // General information
DBG_WARN(category, "message")     // Warning
DBG_ERROR(category, "message")    // Error
```

**With Values:**
```cpp
DBG_VALUE(category, "name", float_value)
DBG_VECTOR3(category, "name", vector)
DBG_BOOL(category, "name", condition)
```

**Assertions:**
```cpp
DBG_ASSERT(condition, category, "failure message")
```

### How Logging Works

**Lazy Initialization:**
The DebugLogger self-initializes on the first `DBG_*` macro call. The macros call `ensure_singleton()` which:
1. Checks if singleton exists
2. If not, creates it and registers with Godot's OS
3. All subsequent calls use the cached singleton

**C++ Code (automatic via macros):**
```cpp
// Macros handle singleton creation and access automatically
DBG_INFO("Category", "message");  // Just works, no setup needed

// Direct access (if needed):
auto* logger = DebugLogger::ensure_singleton();
if (logger) {
  logger->set_log_level(::LogLevel::WARNING);
}
```

**Visual Debugger Access:**
```cpp
auto* debugger = VisualDebugger::get_singleton();
if (debugger && debugger->is_debug_enabled()) {
  debugger->draw_line(p1, p2, Color(1, 0, 0, 1));
}
```

**From GDScript:**
```gdscript
# Access the logger if needed (usually not necessary)
var logger = DebugLogger.get_singleton()
if logger:
  logger.set_log_level(DebugLogger.LogLevel.WARNING)
```

## Visual Debugging Functions

### Drawing Shapes

```cpp
auto* d = VisualDebugger::get_singleton();

// Lines
d->draw_line(from, to, color, thickness);

// Circles (XZ plane)
d->draw_circle_xz(center, radius, color, segments, thickness, filled);

// Spheres (wireframe)
d->draw_sphere(center, radius, color, segments, thickness);

// Boxes
d->draw_box(center, size, color, thickness, filled);

// Vectors with arrows
d->draw_vector(origin, direction, length, color, thickness);

// Position markers
d->draw_cross(center, size, color, thickness);
```

### Clearing Visuals

```cpp
auto* d = VisualDebugger::get_singleton();
d->clear();  // Clear all pending draws
d->set_debug_enabled(false);  // Disable visual debugging
```

## Common Usage Patterns

### Debugging Movement

```cpp
void MovementComponent::_physics_process(double delta) {
  Vector3 current_pos = owner->get_global_position();
  Vector3 target_pos = desired_target;
  Vector3 velocity = (target_pos - current_pos).normalized() * speed;
  
  // Log every few frames
  static int frame_count = 0;
  if (++frame_count % 30 == 0) {
    DBG_VECTOR3("Movement", "velocity", velocity);
  }
  
  // Visual debugging
  auto* d = VisualDebugger::get_singleton();
  if (d && d->is_debug_enabled()) {
    d->draw_vector(current_pos, velocity, 2.0f);
    d->draw_sphere(target_pos, 0.3f, Color(1, 0, 0, 0.5f));
  }
}
```

### Debugging Combat

```cpp
bool AttackComponent::can_attack_target(Unit* target) {
  if (!target) {
    DBG_WARN("Combat", "Target is null");
    return false;
  }
  
  float distance = owner->get_global_position()
    .distance_to(target->get_global_position());
  
  DBG_VALUE("Combat", "distance_to_target", distance);
  DBG_VALUE("Combat", "attack_range", attack_range);
  
  if (distance > attack_range) {
    DBG_DEBUG("Combat", "Target out of range");
    return false;
  }
  
  return true;
}
```

### Debugging Abilities

```cpp
void AbilityComponent::execute_ability(const String& ability_id) {
  DBG_INFO("Abilities", "Executing " + ability_id);
  
  auto* ability = get_ability(ability_id);
  if (!ability) {
    DBG_ERROR("Abilities", "Ability not found: " + ability_id);
    return;
  }
  
  if (!ability->can_cast()) {
    DBG_WARN("Abilities", "Cannot cast " + ability_id);
    return;
  }
  
  DBG_INFO("Abilities", "Cast successful");
  ability->execute();
}
```

## Best Practices

### 1. Use Consistent Categories

```cpp
// Good - consistent naming
DBG_INFO("Movement", "message");
DBG_INFO("Movement", "another message");

// Bad - inconsistent
DBG_INFO("Move", "message");
DBG_INFO("Motion", "another message");
```

Recommended categories:
- Movement
- Combat
- Abilities
- Health
- Physics
- Input
- Interaction
- Resources
- AI
- Debug

### 2. Log State Changes, Not Every Frame

```cpp
// Good - only log on state change
if (state_changed) {
  DBG_INFO("Combat", "State changed to: " + new_state);
}

// Bad - logs every frame
DBG_DEBUG("Combat", "Current state: " + current_state);
```

### 3. Include Meaningful Context

```cpp
// Good - specific and useful
DBG_VALUE("Combat", "damage_dealt", 45);
DBG_VECTOR3("Pathfinding", "target_position", target);

// Bad - too vague
DBG_INFO("System", "Something happened");
DBG_DEBUG("Debug", "x = 5");
```

### 4. Combine Visual and Text Debugging

```cpp
// Log AND visualize
DBG_INFO("Combat", "Attacking " + target->get_name());

auto* d = VisualDebugger::get_singleton();
if (d && d->is_debug_enabled()) {
  d->draw_line(attacker_pos, target_pos, Color(1, 0, 0, 1));
}
```

### 5. Handle Null Logger Gracefully

The macros already do this, but be aware:

```cpp
// Macros check for singleton automatically
DBG_INFO("Category", "message");  // Safe - checks if logger exists

// If accessing directly:
auto* logger = DebugLogger::get_singleton();
if (logger) {  // Always check!
  logger->info("Category", "message");
}
```

## Keyboard Shortcuts

Create a debug_toggle.gd script attached to a node:

```gdscript
extends Node

func _input(event: InputEvent) -> void:
  if not event is InputEventKey or not event.pressed:
    return
  
  match event.keycode:
    # D - Toggle visual debugging
    KEY_D:
      var debugger = VisualDebugger.get_singleton()
      if debugger:
        debugger.set_debug_enabled(!debugger.is_debug_enabled())
    
    # L - Cycle log levels
    KEY_L:
      var logger = DebugLogger.get_singleton()
      if logger:
        var current = logger.get_log_level()
        var next = (current + 1) % 4
        logger.set_log_level(next)
```

## Troubleshooting

### Logs Not Appearing

1. **Check Godot Output Panel:**
   - View > Output (in Godot editor)
   - Make sure it's visible and not scrolled up

2. **Try a simple log message:**
   ```cpp
   DBG_INFO("Test", "Hello");
   ```
   - This initializes the logger if not already done

3. **Check log level threshold:**
   ```cpp
   auto* logger = DebugLogger::ensure_singleton();
   if (logger) {
     logger->set_log_level(::LogLevel::DEBUG);
   }
   ```
   - DEBUG=0 (most verbose), INFO=1, WARNING=2, ERROR=3 (least verbose)

4. **Verify output is enabled:**
   ```cpp
   auto* logger = DebugLogger::ensure_singleton();
   if (logger) {
     logger->set_log_to_output(true);
   }
   ```

### Visual Debugger Not Showing

1. **Check if debugger exists:**
   ```gdscript
   var debugger = VisualDebugger.get_singleton()
   if not debugger:
     print("VisualDebugger not in scene!")
   ```

2. **Check if enabled:**
   ```cpp
   auto* d = VisualDebugger::get_singleton();
   if (!d || !d->is_debug_enabled()) {
     return;  // Not visible
   }
   ```

3. **Add to scene:**
   ```gdscript
   var debugger = VisualDebugger.new()
   add_child(debugger)
   debugger.set_debug_enabled(true)
   ```

## Performance Considerations

- Macros check for null logger before logging (minimal overhead)
- Visual debugging uses ImmediateMesh (no persistent objects created)
- Log level filtering prevents unnecessary processing
- Debug code can be disabled by setting log level to ERROR

## Files Used

- `src/debug/debug_logger.hpp/.cpp` - Logger class
- `src/debug/debug_macros.hpp` - Logging macros
- `src/debug/debug_utils.hpp` - Utility functions
- `src/debug/visual_debugger.hpp/.cpp` - Visual debugging

## See Also

- DEBUG_GUIDE.md - Complete API reference
- DEBUG_EXAMPLES.md - Real-world examples
- DEBUG_SYSTEM_SUMMARY.md - Technical overview

## Quick Reference

```cpp
// Import macros
#include "debug/debug_macros.hpp"

// Log messages
DBG_INFO("Category", "message");
DBG_WARN("Category", "warning");
DBG_ERROR("Category", "error");

// Log with values
DBG_VALUE("Cat", "name", value);
DBG_VECTOR3("Cat", "vec", vector);

// Visual debugging
auto* d = VisualDebugger::get_singleton();
d->draw_line(p1, p2, color);
d->draw_circle_xz(center, radius, color);
d->draw_vector(origin, direction, length);
```

**Ready to debug!** Use this skill whenever you need to add logging or visual debugging to your components.
