# MOBA Game - Comprehensive Debugging Guide

This guide covers the complete debugging system for the MOBA game, including visual debugging and advanced logging capabilities.

**Built on:** Godot's Logger class for native integration and extensibility

## Quick Start

### Visual Debugging

Add debug visualization to your Godot scene:

```gdscript
# In your scene, add a VisualDebugger node
var debugger = VisualDebugger.new()
add_child(debugger)
debugger.set_debug_enabled(true)

# Now use in C++ code:
auto* debugger = VisualDebugger::get_singleton();
debugger->draw_circle_xz(unit_pos, attack_range, Color(1, 0, 0, 0.2f));
debugger->draw_vector(unit_pos, movement_direction, distance);
```

### Logging

```cpp
#include "debug/debug_macros.hpp"

// Simple logging (uses singleton automatically)
DBG_INFO("Movement", "Unit started moving");
DBG_WARN("Combat", "Attack is on cooldown");
DBG_ERROR("Physics", "Collision detection failed");

// Logging with values
DBG_VALUE("Position", "unit_x", unit->get_global_position().x);
DBG_VECTOR3("Movement", "velocity", velocity);
DBG_BOOL("State", "is_alive", unit->is_alive());
```

## Logging System

### DebugLogger Class

The `DebugLogger` **extends Godot's Logger class** for native integration and provides categorized, leveled logging with automatic Godot output routing.

**Features:**
- Extends Godot's Logger class for native integration
- 4 log levels: DEBUG, INFO, WARNING, ERROR
- Category-based filtering for easy issue tracking
- Automatic routing to Godot's print output and error/warning stacks
- Configurable log level threshold
- Singleton pattern for easy access throughout codebase
- Easy-to-use macros with null-safe checks

### Log Levels

| Level   | Value | Usage |
|---------|-------|-------|
| DEBUG   | 0     | Detailed diagnostic information (most verbose) |
| INFO    | 1     | General informational messages |
| WARNING | 2     | Warning messages (also uses Godot's push_warning) |
| ERROR   | 3     | Error messages (also uses Godot's push_error) |

### Using DebugLogger in C++

**Basic Usage (via Singleton):**

```cpp
#include "debug/debug_logger.hpp"

auto* logger = DebugLogger::get_singleton();
if (logger) {
  logger->info("MySystem", "Starting initialization");
  logger->warning("Combat", "Attack out of range");
  logger->error("Physics", "Invalid collision shape");
  logger->debug("Movement", "Pathfinding calculation took 2ms");
}
```

**Control Log Level:**

```cpp
auto* logger = DebugLogger::get_singleton();
if (logger) {
  // Only log WARNING and ERROR messages
  logger->set_log_level(DebugLogger::LogLevel::WARNING);
  
  // Log everything (default)
  logger->set_log_level(DebugLogger::LogLevel::DEBUG);
  
  // Check current level
  int current = logger->get_log_level();
}
```

**Toggle Output:**

```cpp
auto* logger = DebugLogger::get_singleton();
if (logger) {
  // Disable console output (still uses Godot's push_warning/push_error)
  logger->set_log_to_output(false);
  
  // Re-enable
  logger->set_log_to_output(true);
}
```

> **Note:** The macros handle singleton lookup automatically, so you usually don't need to do this manually.

### Using Debug Macros

Macros provide a convenient shorthand for common logging patterns:

**Simple Messages:**

```cpp
#include "debug/debug_macros.hpp"

DBG_DEBUG("Category", "Debug message");
DBG_INFO("Category", "Info message");
DBG_WARN("Category", "Warning message");
DBG_ERROR("Category", "Error message");
```

**Logging Values:**

```cpp
// Single float/int value
DBG_VALUE("Movement", "speed", unit->get_speed());

// Vector3 with automatic formatting
DBG_VECTOR3("Position", "unit_location", unit->get_global_position());

// Boolean value
DBG_BOOL("State", "is_alive", health > 0);
```

**Debug-Only Assertions:**

```cpp
// Only logs in debug builds (when NDEBUG is not defined)
DBG_DEBUG_ONLY("Movement", "Frame-by-frame debug info");
```

**Assertions with Logging:**

```cpp
DBG_ASSERT(ptr != nullptr, "Pointer", "ptr must not be null");
DBG_ASSERT(health >= 0, "Health", "Health should never be negative");
```

**Example in a Component:**

```cpp
// In your component's _ready() method
void MyComponent::_ready() {
  DBG_INFO("MyComponent", "Initialization started");
  
  // Do initialization...
  
  if (!is_ready) {
    DBG_ERROR("MyComponent", "Failed to initialize");
    return;
  }
  
  DBG_INFO("MyComponent", "Initialization complete");
}

// In _process()
void MyComponent::_process(double delta) {
  DBG_DEBUG("MyComponent", "Processing frame");
  
  if (some_error_condition) {
    DBG_WARN("MyComponent", "Unexpected state detected");
  }
}
```

## Visual Debugging System

The `VisualDebugger` provides real-time visualization of debug information directly in the Godot viewport.

### VisualDebugger Class

**Features:**
- Draw geometric shapes (lines, circles, spheres, boxes)
- Visual range indicators
- Movement path visualization
- Direction vectors with arrows
- Real-time frame-by-frame updates
- Enable/disable with toggle

### Drawing Functions

**Lines:**

```cpp
auto* debugger = VisualDebugger::get_singleton();

debugger->draw_line(
  Vector3(0, 0, 0),        // from
  Vector3(10, 0, 0),       // to
  Color(1, 0, 0, 1),       // color (red)
  2.0f                     // thickness
);
```

**Circles (XZ plane):**

```cpp
debugger->draw_circle_xz(
  Vector3(5, 1, 5),        // center
  3.0f,                    // radius
  Color(0, 1, 0, 1),       // color (green)
  32,                      // segments
  1.0f,                    // thickness
  false                    // filled
);
```

**Spheres (wireframe):**

```cpp
debugger->draw_sphere(
  unit_pos,                // center
  2.0f,                    // radius
  Color(0, 0, 1, 0.5f),    // color (semi-transparent blue)
  16,                      // segments
  1.0f                     // thickness
);
```

**Vectors with Arrows:**

```cpp
debugger->draw_vector(
  unit_pos,                // origin
  movement_direction,      // direction (normalized)
  distance,                // length
  Color(1, 1, 0, 1),       // color (yellow)
  1.5f                     // thickness
);
```

**Cross Marker:**

```cpp
debugger->draw_cross(
  Vector3(5, 1, 5),        // center
  1.0f,                    // size
  Color(1, 0, 0, 1),       // color (red)
  2.0f                     // thickness
);
```

**Boxes:**

```cpp
debugger->draw_box(
  Vector3(5, 1, 5),        // center
  Vector3(2, 2, 2),        // size (width, height, depth)
  Color(1, 1, 1, 1),       // color (white)
  1.0f,                    // thickness
  false                    // filled
);
```

### Debug Utilities

The `DebugUtils` namespace provides helper functions for common debug visualization patterns:

**Draw Unit State:**

```cpp
#include "debug/debug_utils.hpp"

DebugUtils::draw_unit_debug(
  unit,                         // Unit pointer
  forward_direction,            // Direction unit is facing
  0.5f                         // Debug marker size
);
```

This draws:
- A red cross at the unit's position
- A green vector showing forward direction
- A blue circle showing the unit's footprint

**Draw Paths:**

```cpp
Vector3 path_points[5] = {...};
DebugUtils::draw_path(
  path_points,        // Array of waypoints
  5,                  // Number of points
  0.2f,              // Radius of each waypoint marker
  Color(0, 1, 0, 1), // Color (green)
  1.0f               // Thickness
);
```

**Draw Range Indicators:**

```cpp
// Attack range (red)
DebugUtils::draw_attack_range(unit_pos, attack_range);

// Vision range (cyan)
DebugUtils::draw_vision_range(unit_pos, vision_range);

// Custom range
DebugUtils::draw_range_indicator(
  center_pos,
  radius,
  Color(1, 0, 1, 0.2f),  // Magenta, semi-transparent
  2.0f
);
```

**Log Unit State:**

```cpp
DebugUtils::log_unit_state(
  "Combat",
  unit,
  "hp=85/100 state=attacking"
);

// Output: [DEBUG] [Combat] Unit001 at (5.32, 1.00, -3.14) | hp=85/100 state=attacking
```

## Practical Examples

### Debugging Movement

```cpp
// In your movement component
void MovementComponent::_physics_process(double delta) {
  DBG_DEBUG("Movement", "Physics frame started");
  
  // Get current position
  Vector3 current_pos = owner->get_global_position();
  
  // Calculate desired velocity
  Vector3 velocity = (desired_target - current_pos).normalized() * speed;
  
  // Log for tracking
  DBG_VALUE("Movement", "speed", speed);
  DBG_VECTOR3("Movement", "velocity", velocity);
  
  // Apply movement
  owner->set_velocity(velocity);
  owner->move_and_slide();
  
  // Visualize
  auto* debugger = VisualDebugger::get_singleton();
  if (debugger && debugger->is_debug_enabled()) {
    debugger->draw_vector(current_pos, velocity, 2.0f);
    debugger->draw_sphere(desired_target, 0.5f, Color(1, 0, 0, 0.5f));
  }
}
```

### Debugging Combat

```cpp
// In your attack component
bool AttackComponent::can_attack_target(Unit* target) {
  DBG_INFO("Combat", "Checking if can attack target");
  
  if (!target) {
    DBG_ERROR("Combat", "Target is null");
    return false;
  }
  
  Vector3 distance_vec = target->get_global_position() - owner->get_global_position();
  float distance = distance_vec.length();
  
  DBG_VALUE("Combat", "distance_to_target", distance);
  DBG_VALUE("Combat", "attack_range", attack_range);
  
  bool in_range = distance <= attack_range;
  DBG_BOOL("Combat", "in_range", in_range);
  
  if (!in_range) {
    DBG_WARN("Combat", "Target out of range, moving to attack");
    // Start chasing...
  }
  
  return in_range;
}
```

### Debugging Pathfinding

```cpp
// In your pathfinding system
void PathfindingSystem::find_path(const Vector3& start, const Vector3& end) {
  DBG_INFO("Pathfinding", "Starting pathfinding calculation");
  
  auto* debugger = VisualDebugger::get_singleton();
  
  // Run pathfinding algorithm
  std::vector<Vector3> path = calculate_path(start, end);
  
  if (path.empty()) {
    DBG_ERROR("Pathfinding", "No path found between start and end");
    return;
  }
  
  DBG_INFO("Pathfinding", "Found path with " + String::num(path.size()) + " waypoints");
  
  // Visualize the path
  if (debugger && debugger->is_debug_enabled()) {
    for (size_t i = 0; i < path.size(); ++i) {
      debugger->draw_sphere(path[i], 0.3f, Color(0, 1, 0, 1), 8);
      
      if (i < path.size() - 1) {
        debugger->draw_line(path[i], path[i + 1], Color(0, 1, 0, 1), 2.0f);
      }
    }
  }
}
```

### Debugging Abilities

```cpp
// In your ability system
void AbilityComponent::execute_ability(const String& ability_id) {
  DBG_INFO("Abilities", "Executing ability: " + ability_id);
  
  auto* ability = get_ability(ability_id);
  if (!ability) {
    DBG_ERROR("Abilities", "Ability not found: " + ability_id);
    return;
  }
  
  // Check prerequisites
  if (!ability->can_cast()) {
    DBG_WARN("Abilities", "Cannot cast " + ability_id + " (cooldown/mana)");
    return;
  }
  
  DBG_INFO("Abilities", "Ability " + ability_id + " cast successfully");
  ability->execute();
}
```

## Setting Log Levels at Runtime

You can change log levels through GDScript in your scene:

```gdscript
# In your Godot scene script
func _ready():
  var logger = DebugLogger.new()
  
  # Only show warnings and errors
  logger.set_log_level(DebugLogger.LogLevel.WARNING)
  
  # Show everything
  logger.set_log_level(DebugLogger.LogLevel.DEBUG)
  
  # Disable console output (but keep Godot's error/warning system)
  logger.set_log_to_output(false)

func _input(event):
  if event is InputEventKey and event.pressed:
    if event.keycode == KEY_D:  # Press 'D' to toggle debug
      var debugger = VisualDebugger.get_singleton()
      if debugger:
        debugger.set_debug_enabled(!debugger.is_debug_enabled())
```

> **Important:** Create a DebugLogger instance in your scene (usually in _ready()) or the singleton will be null.

## Output Examples

### Console Output

```
[INFO] [Movement] Unit started moving
[DEBUG] [Movement] velocity: (5.12, 0.00, -3.45)
[DEBUG] [Combat] distance_to_target: 2.34
[WARNING] [Combat] Target out of range, moving to attack
[ERROR] [Physics] Invalid collision shape
```

### Godot Output Panel

The Output panel at the bottom of Godot editor will show:
- Regular print messages from `DBG_DEBUG()`, `DBG_INFO()`
- **[WARNING]** messages from `DBG_WARN()` (with orange icon)
- **[ERROR]** messages from `DBG_ERROR()` (with red icon)

### Visual Output

In the 3D viewport:
- Green circles show attack/vision ranges
- Red crosses mark unit positions
- Yellow vectors show movement directions
- Blue spheres show detection areas
- White lines show paths

## Best Practices

### 1. Use Categories Consistently

```cpp
// Good - consistent categories
DBG_INFO("Movement", "Unit reached destination");
DBG_WARN("Movement", "No path available");
DBG_ERROR("Movement", "Navigation agent failed");

// Bad - inconsistent/vague categories
DBG_INFO("Unit", "Something happened");
DBG_WARN("System", "That's weird");
```

### 2. Log Meaningful Information

```cpp
// Good - specific and useful
DBG_VALUE("Combat", "damage_dealt", 45);
DBG_VECTOR3("Pathfinding", "target_position", target_pos);

// Bad - too vague
DBG_INFO("System", "did something");
DBG_DEBUG("Debug", "x = 5");
```

### 3. Use Appropriate Log Levels

```cpp
// DEBUG - detailed diagnostic info (enabled during development)
DBG_DEBUG("Movement", "Frame: calculated velocity");

// INFO - informational but significant events
DBG_INFO("Combat", "Unit killed enemy");

// WARNING - something unexpected but recoverable
DBG_WARN("AI", "Target became invalid, finding new target");

// ERROR - something went seriously wrong
DBG_ERROR("Physics", "CollisionShape3D not found");
```

### 4. Combine Visual and Text Debugging

```cpp
void AttackComponent::attack(Unit* target) {
  auto* debugger = VisualDebugger::get_singleton();
  
  // Visual indicator
  if (debugger && debugger->is_debug_enabled()) {
    debugger->draw_line(owner->get_global_position(), 
                       target->get_global_position(),
                       Color(1, 0, 0, 1), 2.0f);
  }
  
  // Text log
  DBG_INFO("Combat", "Attacking " + target->get_name());
}
```

### 5. Performance Considerations

```cpp
// Don't log in tight loops during release
void Movement::_process(double delta) {
  // Only log on state changes
  if (state_changed) {
    DBG_INFO("Movement", "State changed to: " + current_state);
  }
  
  // Use debug-only logging for frame-by-frame info
  DBG_DEBUG_ONLY("Movement", "Processing frame");
}
```

## Troubleshooting

### Messages not appearing in console

**Solution:** Check log level threshold:
```cpp
if (DebugLogger::get_log_level() > LogLevel::DEBUG) {
  DebugLogger::set_log_level(LogLevel::DEBUG);
}
```

### Visual debugger not showing

**Solution:** Ensure VisualDebugger exists and is enabled:
```gdscript
var debugger = VisualDebugger.get_singleton()
if debugger:
  debugger.set_debug_enabled(true)
else:
  print("VisualDebugger not found in scene!")
```

### Warnings/Errors in Error panel not showing

**Solution:** Ensure logging is enabled:
```cpp
DebugLogger::set_log_to_output(true);
```

## Summary

The MOBA debugging system provides:

1. **DebugLogger** - Categorized, leveled logging using Godot's native systems
2. **Debug Macros** - Easy-to-use shortcuts for common logging patterns
3. **VisualDebugger** - Real-time geometric visualization
4. **Debug Utils** - Helper functions for common debugging tasks

Use these tools together to quickly identify and fix issues during development!
