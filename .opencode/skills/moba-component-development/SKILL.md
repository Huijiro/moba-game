---
name: moba-component-development
description: Develop MOBA components following the established architecture pattern
license: MIT
compatibility: opencode
metadata:
  audience: developers
  workflow: component-design
  project: moba-game
---

## What I Do
- Guide implementation of new UnitComponents for the MOBA
- Follow the established patterns in HealthComponent and AttackComponent
- Handle Godot bindings correctly (ClassDB, properties, signals)
- Ensure components integrate with the Unit class properly

## When to Use Me

Use this skill when implementing any new component in `src/components/`:
- Creating a new system (AbilityComponent, StatusEffectsComponent, etc)
- Extending existing components with new features
- Setting up proper Godot bindings

## The MOBA Component Pattern

### File Structure
```
src/components/
├── unit_component.hpp/.cpp     # Base class
├── health/
│   ├── health_component.hpp
│   └── health_component.cpp
├── combat/
│   ├── attack_component.hpp
│   └── attack_component.cpp
└── CMakeLists.txt
```

### Base Class Pattern (UnitComponent)
```cpp
// unit_component.hpp
#pragma once
#include <godot_cpp/classes/node3d.hpp>

class Unit;  // Forward declare

class UnitComponent : public godot::Node3D {
    GDCLASS(UnitComponent, godot::Node3D)

protected:
    Unit* owner_unit = nullptr;

public:
    static void _bind_methods();
    virtual void _enter_tree() override;
    
    Unit* get_owner_unit() const { return owner_unit; }
};
```

### Implementation Pattern (HealthComponent Example)
```cpp
// health_component.hpp
class HealthComponent : public UnitComponent {
    GDCLASS(HealthComponent, UnitComponent)

private:
    float current_health = 100.0f;
    float max_health = 100.0f;

protected:
    static void _bind_methods();

public:
    void take_damage(float damage);
    float get_current_health() const;
    void set_max_health(float health);
    bool is_alive() const;
};
```

## Key Implementation Steps

### 1. Create Header File
- Extend UnitComponent
- Declare `GDCLASS(ClassName, UnitComponent)`
- Add member variables (private)
- Declare setter/getter methods

### 2. Implement Binding
```cpp
void HealthComponent::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD("set_max_health", "health"),
        &HealthComponent::set_max_health
    );
    ClassDB::bind_method(
        D_METHOD("get_current_health"),
        &HealthComponent::get_current_health
    );
    ADD_PROPERTY(
        PropertyInfo(godot::Variant::FLOAT, "max_health"),
        "set_max_health", "get_current_health"
    );
    ADD_SIGNAL(MethodInfo("health_changed", 
        PropertyInfo(godot::Variant::FLOAT, "new_health")));
}
```

### 3. Handle Initialization
- Get owner Unit reference in `_enter_tree()`
- Check `is_inside_tree()` before using owner
- Wait for parent to be ready if needed

```cpp
void HealthComponent::_enter_tree() {
    owner_unit = Object::cast_to<Unit>(get_parent());
    if (!owner_unit) {
        UtilityFunctions::push_error(
            "[HealthComponent] Parent must be a Unit!");
    }
}
```

### 4. Register in CMakeLists.txt
Add component to the build system:
```cmake
# src/components/combat/CMakeLists.txt
target_sources(
    GodotGame PRIVATE
    attack_component.hpp
    attack_component.cpp
    projectile.hpp
    projectile.cpp
)
```

### 5. Register in register_types.cpp
Add to the GDExtension registration:
```cpp
ClassDB::register_class<HealthComponent>();
ClassDB::register_class<AttackComponent>();
```

## Common Patterns

### Using Godot's Logging
```cpp
#include <godot_cpp/variant/utility_functions.hpp>
using godot::UtilityFunctions;

UtilityFunctions::print("[HealthComponent] Taking damage: " + 
    godot::String::num(damage));
```

### Safe Node Casting
```cpp
Unit* owner = Object::cast_to<Unit>(get_parent());
if (!owner) {
    UtilityFunctions::push_error("Invalid parent!");
    return;
}
```

### Exposing Signals
```cpp
// Header
class AttackComponent : public UnitComponent {
    // Declare signal in _bind_methods:
    // ADD_SIGNAL(MethodInfo("attack_performed", ...));
};

// Usage in code
emit_signal("attack_performed", target, damage);
```

## Architecture Integration

### How Components Connect to Unit
1. Component is added as child of Unit in scene
2. `_enter_tree()` gets reference to Unit parent
3. Component can call Unit methods or access its state
4. Unit doesn't directly know about components (loose coupling)

### Example: AttackComponent Using Unit
```cpp
void AttackComponent::perform_attack(Unit* target) {
    if (!owner_unit || !target) return;
    
    // Use owner_unit for position/reference
    float distance = owner_unit->get_global_position()
        .distance_to(target->get_global_position());
    
    if (distance <= attack_range) {
        target->take_damage(attack_damage);  // Assumes Unit has this
    }
}
```

### Example: Unit Integrating Components
```cpp
// In Unit class
HealthComponent* health_comp = get_node<HealthComponent>("HealthComponent");
if (health_comp) {
    health_comp->take_damage(10);
}
```

## Scene Setup in Godot

Typical scene hierarchy:
```
Unit (C++ class)
├── NavigationAgent3D
├── MeshInstance3D
├── CollisionShape3D
├── HealthComponent (child node)
├── AttackComponent (child node)
├── MovementComponent (child node)
└── ... other components
```

Each component:
- Is a child Node3D of Unit
- Gets owner_unit in `_enter_tree()`
- Operates independently but can access owner_unit

## Testing Components

When testing a new component:
1. Add test node to GodotGame/main.tscn
2. Verify component appears in Inspector
3. Test properties can be set/get
4. Check signals emit correctly
5. Verify it doesn't break existing systems

## Common Issues

**"Cannot cast parent to Unit"**
- Check component is actually a child of Unit in scene
- Verify Unit node type is correct

**"Property not showing in Inspector"**
- Verify `ADD_PROPERTY` called in `_bind_methods()`
- Check property type matches Variant::* enum
- Rebuild and reload scene

**"Null reference errors"**
- Always check `owner_unit != nullptr` before using
- Verify `_enter_tree()` is called before other methods
- Use `is_inside_tree()` checks

## Design Tips

1. **Single Responsibility**: Each component owns one system
2. **Loose Coupling**: Don't hardcode references to other components
3. **Expose via Properties**: Let Godot editor configure values
4. **Use Signals**: Emit signals for important events
5. **Meaningful Names**: Make purpose clear (AttackComponent, not DamageComponent)
