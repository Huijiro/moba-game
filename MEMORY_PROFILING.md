# Memory Profiling System

## Overview

The memory profiling system provides real-time tracking of resource allocations and deallocations throughout the game. This is essential for identifying memory leaks, tracking object lifetime patterns, and validating cleanup effectiveness.

## Architecture

### Components

#### 1. MemoryProfiler (Core Engine)
**Location:** `src/debug/memory_profiler.hpp/cpp`

Static utility class that tracks allocations by category.

**Key Features:**
- Records allocation/deallocation events with timestamps
- Categorizes objects (e.g., "VFXNode", "MeshInstance3D")
- Calculates statistics automatically
- Thread-safe static state

**API:**
```cpp
// Track new allocation
MemoryProfiler::track_allocation("VFXNode", "VFXNode_1_0x7f1234");

// Track deallocation
MemoryProfiler::track_deallocation("VFXNode", "VFXNode_1_0x7f1234");

// Get statistics for a category
auto stats = MemoryProfiler::get_category_stats("VFXNode");
// Returns: allocated_count, freed_count, currently_alive, peak_alive, avg_lifetime_ms

// Get all stats as Dictionary (for UI display)
Dictionary all_stats = MemoryProfiler::get_all_stats_dict();

// Get detailed records for debugging
Dictionary details = MemoryProfiler::get_detailed_records("VFXNode");
// Returns: {"alive": [...], "freed": [...]}

// Get alive allocations by category
Dictionary alive = MemoryProfiler::get_alive_allocations();

// Get summary report as string
String report = MemoryProfiler::get_summary_report();

// Control profiling
MemoryProfiler::set_enabled(true);  // Enable/disable tracking
MemoryProfiler::clear();  // Clear all records
```

#### 2. MemoryDisplayComponent (UI)
**Location:** `src/components/ui/memory_display_component.hpp/cpp`

Godot Control node that displays memory statistics in-game.

**Features:**
- Real-time updating with configurable interval
- Styled panel with semi-transparent background
- Per-category statistics display
- Automatic warnings for high allocation counts
- Can be toggled on/off from editor

**Properties:**
```
show_stats (bool) - Toggle display on/off
update_interval (float) - Seconds between updates (default: 0.5)
```

**Scene Setup:**
Added to `GodotGame/ui/game_ui.tscn` as a child of Screen:
- Positioned: Top-left corner (10px from edges)
- Size: 400x300px
- Visible by default

### Integration Points

#### VFXNode Tracking
**File:** `src/visual/vfx_node.hpp/cpp`

Each VFXNode instance tracks itself:

```cpp
VFXNode::VFXNode() {
  // Generate unique ID: "VFXNode_123_0x12345678"
  std::ostringstream oss;
  oss << "VFXNode_" << (++vfx_instance_count) << "_" << this;
  profiler_id = oss.str();
  
  // Register allocation
  MemoryProfiler::track_allocation("VFXNode", profiler_id);
}

VFXNode::~VFXNode() {
  // Register deallocation
  MemoryProfiler::track_deallocation("VFXNode", profiler_id);
}
```

## Data Flow

### Allocation Tracking
```
1. VFXNode created
   ↓
2. Unique ID generated
   ↓
3. MemoryProfiler::track_allocation() called
   ↓
4. Record added to allocations map
   ↓
5. Statistics recalculated
```

### UI Update Cycle
```
1. MemoryDisplayComponent._process() called each frame
   ↓
2. elapsed_time += delta
   ↓
3. If elapsed_time >= update_interval:
   ↓
4. MemoryProfiler::get_all_stats_dict() called
   ↓
5. MemoryProfiler::get_alive_allocations() called
   ↓
6. Label text updated with formatted statistics
   ↓
7. Warnings added for high counts (alive > 10)
```

## Statistics Explained

### Per-Category Stats
```
allocated:  Total number of objects created in this category
freed:      Total number of objects destroyed
alive:      Currently active objects (allocated - freed)
peak:       Maximum simultaneous alive objects
avg_lifetime_ms: Average lifetime in milliseconds for freed objects
```

### Example Output
```
=== Memory Stats ===

VFXNode:
  Allocated: 15
  Freed: 12
  Alive: 3
  Peak: 5
  Avg Lifetime: 1250.5 ms
```

**Interpretation:**
- 15 VFX nodes were created during gameplay
- 12 have been successfully cleaned up
- 3 are currently active (expected during ability casting)
- Peak of 5 simultaneous nodes (from rapid ability casting)
- Average node lifetime: 1.25 seconds

## Usage Patterns

### Finding Leaks During Gameplay

1. **Start fresh session:**
   - Scene loads, MemoryProfiler initializes
   - Watch memory display in top-left corner

2. **Cast multiple abilities:**
   - VFX allocations appear in "Alive" count
   - Watch count return to 0 as effects finish
   - If count stays elevated, potential leak detected

3. **Identify problem areas:**
   - Note which category has high alive count
   - Check if numbers match expected behavior
   - Use detailed records to find long-lived objects

### Expected Behavior

**Explosion Ability (1 second duration):**
```
Before cast: Alive: 0
During cast: Alive: 1 (explosion + mesh)
After 1s:    Alive: 0 (cleaned up)
Total:       Allocated: N, Freed: N ✓
```

**Fireball Ability (projectile + impact):**
```
Before cast: Alive: 0
During cast: Alive: 1-2 (projectile + vfx)
On impact:   Alive: 1 (explosion effect)
After 1s:    Alive: 0 (all cleaned up)
Total:       Allocated: M, Freed: M ✓
```

### Leak Indicators

**Red Flags:**
- Alive count increases with each ability cast (never decreases)
- Peak alive count grows over time
- Freed count doesn't match expected pattern
- Allocated >> Freed with no corresponding alive objects

**Example Leak Pattern:**
```
Cast 1: Alive: 1
Cast 2: Alive: 2  (should be 1)
Cast 3: Alive: 3  (should be 1)
Cast 4: Alive: 4  (should be 1)
→ Objects not being freed!
```

## Advanced Debugging

### Detailed Records
For deep investigation, access detailed allocation records:

```cpp
// Get detailed records for a category
Dictionary details = MemoryProfiler::get_detailed_records("VFXNode");

// alive: Array of currently active allocations
Array alive = details["alive"];
// Each entry: {name, timestamp, age_ms}

// freed: Array of freed allocations
Array freed = details["freed"];
// Each entry: {name, timestamp, lifetime_ms}
```

### Summary Report
Get a text-based summary for logging:

```cpp
String report = MemoryProfiler::get_summary_report();
// Prints to debug output via GDScript or print_debug()
```

## Performance Considerations

### Memory Overhead
- Per allocation: ~150 bytes (string IDs + timestamps + state)
- Typical session: 100-1000 allocations = 15KB-150KB
- Negligible impact on game performance

### CPU Overhead
- track_allocation(): O(1) map insertion
- track_deallocation(): O(n) LIFO search (rarely n > 20)
- get_all_stats_dict(): O(n) iteration
- UI update every 0.5s (not every frame)

### Optimization
Profiling can be disabled at runtime if needed:
```cpp
MemoryProfiler::set_enabled(false);  // No tracking overhead
```

## Integration with Other Systems

### Debug Logging
Memory profiler works alongside debug logging:
```cpp
DBG_INFO("VFXNode", "Explosion spawned");  // Debug log
MemoryProfiler::track_allocation("VFXNode", id);  // Memory tracking
```

### Visual Debugger
Can be used with VisualDebugger to show 3D visualization:
```cpp
// Draw active VFX locations
for (auto& alloc : alive_allocations["VFXNode"]) {
  // Could extend to track position and draw markers
}
```

## Future Enhancements

### Potential Improvements
1. **Category hierarchy:** Track "VFX/Explosion", "VFX/Projectile" separately
2. **Memory size tracking:** Record actual byte allocations
3. **Heatmaps:** Visualize allocation patterns over time
4. **Breakpoints:** Pause on specific allocation patterns
5. **Per-frame view:** Show allocations per frame instead of total
6. **Export:** Save profiling data to CSV/JSON for analysis

### Adding New Tracked Categories

To track a new class:

```cpp
// In constructor
profiler_id = generate_unique_id();
MemoryProfiler::track_allocation("MyClass", profiler_id);

// In destructor
MemoryProfiler::track_deallocation("MyClass", profiler_id);

// In UI (automatic - reads from MemoryProfiler::get_all_stats_dict())
```

## Troubleshooting

### Display Not Appearing
1. Check show_stats property is true
2. Verify MemoryDisplayComponent is in scene
3. Check CanvasLayer is above other UI elements

### Numbers Look Wrong
1. Ensure profiler is enabled: `MemoryProfiler::is_enabled()`
2. Check timestamps are being recorded
3. Verify update_interval isn't too long

### Memory Stats Always Zero
1. Confirm VFXNode instances are being created
2. Check profiler_id generation isn't failing
3. Verify track_allocation() is being called
4. Look for exceptions in Godot output

## Summary

The memory profiling system provides:
- ✓ Real-time allocation tracking
- ✓ Visual display of memory statistics
- ✓ Automatic leak detection
- ✓ Minimal performance overhead
- ✓ Easy integration with existing systems
- ✓ Extensible for new categories

This enables confident validation that memory cleanup is working correctly throughout the VFX system.
