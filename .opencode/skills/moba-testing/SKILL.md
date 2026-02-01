---
name: moba-testing
description: Build, test, and validate MOBA components and systems
license: MIT
compatibility: opencode
metadata:
  audience: developers
  workflow: testing
  project: moba-game
---

## What I Do
- Guide the build and test process for the MOBA
- Help debug compilation and runtime issues
- Verify components work correctly in the engine
- Ensure changes don't break existing systems

## When to Use Me

Use this skill when you need to:
- Build the C++ code after making changes
- Debug build failures or runtime errors
- Verify a component works in Godot
- Check that changes didn't break other systems
- Test against acceptance criteria

## Build Process

### Prerequisites
- CMake 3.30+
- Ninja build system
- C++17 compatible compiler (GCC/Clang)
- Godot 4.6 editor

### Building

**Debug Build (Recommended for Development)**
```bash
cmake -S . -DCMAKE_BUILD_TYPE=Debug
ninja
```

**Release Build (Optimized)**
```bash
cmake -S . -DCMAKE_BUILD_TYPE=Release
ninja
```

**Clean Rebuild**
```bash
rm -rf CMakeFiles build.ninja CMakeCache.txt
cmake -S . -DCMAKE_BUILD_TYPE=Debug
ninja
```

### Build Output
- Debug library: `GodotGame/lib/Linux-x86_64/libGodotGame-d.so`
- Release library: `GodotGame/lib/Linux-x86_64/libGodotGame.so`

## Testing Workflow

### 1. After Making Code Changes
```bash
# Rebuild
ninja

# Checks:
# - Code compiles without errors
# - No linker issues
# - Library builds successfully
```

### 2. Manual Testing in Godot
1. Open GodotGame/main.tscn in Godot 4.6 editor
2. Look for new properties in Inspector
3. Run the scene (F5 or play button)
4. Verify component behavior matches requirements
5. Check debug output for any errors

### 3. Testing Common Scenarios

**For Combat Components:**
- Can unit attack another unit?
- Does cooldown prevent rapid attacks?
- Is damage applied correctly?
- Does range checking work?

**For Movement Components:**
- Does unit navigate to target?
- Does it avoid obstacles?
- Does navigation complete properly?

**For Health Components:**
- Does health decrease when damaged?
- Does dead unit stop moving?
- Are signals emitted correctly?

## Common Build Errors

### Error: "No member named 'xxx' in namespace 'godot'"
**Cause**: Missing include file

**Fix**: Add `#include <godot_cpp/classes/xxx.hpp>` or `#include <godot_cpp/variant/xxx.hpp>`

### Error: "File overridden by precompiled header"
**Cause**: Need clean rebuild

**Fix**: `rm -rf CMakeFiles CMakeCache.txt && cmake -S . && ninja`

### Error: "Linking undefined references"
**Cause**: Source file not added to CMakeLists.txt

**Fix**: Check CMakeLists.txt has target_sources with your new files

## Runtime Issues

### "Condition '!is_inside_tree()' is true"
**Cause**: Node used before being added to scene tree

**Fix**: Ensure parent->add_child(node) before set_global_position or other methods

### "No navigation mesh"
**Cause**: Navigation region not baked

**Fix**:
1. Open main.tscn in Godot
2. Select NavigationRegion3D
3. Click "Bake Mesh" in Inspector
4. Reload scene

### Component not showing in Inspector
**Cause**: Property not bound in _bind_methods() or class not registered

**Fix**:
1. Verify ADD_PROPERTY in _bind_methods()
2. Verify class registered in register_types.cpp
3. Rebuild and reload scene

## Verification Checklist

When completing a task, verify:

- [ ] **Code Compiles**: `ninja` succeeds with no errors
- [ ] **All Properties Bound**: Properties show in Godot Inspector
- [ ] **Meets Acceptance Criteria**: Each criterion from issue is tested
- [ ] **No Regressions**: Existing systems still work
- [ ] **Debug Output**: Check for unexpected errors in console
- [ ] **Godot Scene Works**: Scene plays without crashing
- [ ] **Clean Commits**: Code is organized in logical commits

## Performance Considerations

**Monitor for Bottlenecks:**
- Navigation queries every frame - may need optimization
- Raycasts for input - relatively cheap but avoid in hot loops
- Component updates - keep _physics_process logic simple

**Quick Performance Check:**
- Open Debugger in Godot
- Check frame time for large operations
- Profile if suspected bottleneck

## Debug Logging

When debugging, use Godot's logging (not std::cout):

```cpp
#include <godot_cpp/variant/utility_functions.hpp>
using godot::UtilityFunctions;
using godot::String;

// Print to console
UtilityFunctions::print("Debug: " + String::num(value));

// Print error
UtilityFunctions::push_error("Error occurred!");

// Print warning
UtilityFunctions::push_warning("Watch out!");
```

Output appears in Godot's Output panel.

## Scene Testing Strategy

### Minimal Test Setup
```
Node3D (root)
├── NavigationRegion3D
│   └── StaticBody3D (ground)
├── Unit (with components)
├── MOBACamera
└── InputManager
```

### Testing Component Isolation
Test each component independently:
1. Create minimal scene with just the component
2. Set properties in Inspector
3. Watch output and behavior
4. Verify it doesn't depend on other systems

### Integration Testing
Test components working together:
1. Use full main.tscn
2. Verify all systems interact correctly
3. Test complex scenarios (multiple units, combat, etc)

## Acceptance Criteria Mapping

When verifying a task, map each criterion to a test:

Example: Task #4 "Implement attack cooldown system"
- [ ] Cooldown timer implemented → Check timer variable in code
- [ ] Attack checks cooldown → Verify attack_ready() before firing
- [ ] Cooldown resets after attack → Time attack interval
- [ ] Property exposed to editor → See it in Inspector
- [ ] Works with different durations → Test with 0.5s, 2s, etc

## Tips for Effective Testing

1. **Test Early**: Don't wait until all code is done
2. **Incremental**: Build a little, test a little
3. **Automate**: Write reusable test scenarios
4. **Document**: Note how to reproduce issues
5. **Keep Records**: Remember what tests pass/fail
