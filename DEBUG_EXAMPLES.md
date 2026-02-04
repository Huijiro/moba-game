# Debug System - Practical Examples

Complete, copy-paste-ready examples for using the debugging system.

## Example 1: Setup in Main Scene

```gdscript
# main.gd - Main scene script
extends Node3D

func _ready():
  # Initialize debug logger (must be done before any C++ logging)
  var logger = DebugLogger.new()
  logger.set_log_level(DebugLogger.LogLevel.DEBUG)
  logger.set_log_to_output(true)
  
  # Print confirmation
  print("Debug system initialized")
  
  # Optional: Initialize visual debugger
  var visual_debugger = VisualDebugger.new()
  add_child(visual_debugger)
  visual_debugger.set_debug_enabled(true)
  
  print("Visual debugger initialized")

func _input(event: InputEvent) -> void:
  if event is InputEventKey and event.pressed:
    match event.keycode:
      # Press 'D' to toggle visual debug mode
      KEY_D:
        var debugger = VisualDebugger.get_singleton()
        if debugger:
          var enabled = debugger.is_debug_enabled()
          debugger.set_debug_enabled(!enabled)
          print("Visual debug: %s" % ("ON" if !enabled else "OFF"))
      
      # Press 'L' to cycle log levels
      KEY_L:
        var logger = DebugLogger.get_singleton()
        if logger:
          var current = logger.get_log_level()
          var next = (current + 1) % 4  # Cycle through 0-3
          logger.set_log_level(next)
```

## Example 2: Movement Component with Debugging

```cpp
// movement_component.cpp
#include "movement_component.hpp"
#include "debug/debug_macros.hpp"

void MovementComponent::_ready() {
  DBG_INFO("Movement", "Component initialized");
  owner_unit = Object::cast_to<Unit>(get_parent());
  
  if (!owner_unit) {
    DBG_ERROR("Movement", "Parent is not a Unit");
    return;
  }
  
  DBG_INFO("Movement", "Owner unit: " + owner_unit->get_name());
}

void MovementComponent::_physics_process(double delta) {
  if (!owner_unit || !owner_unit->get_desired_location()) {
    return;
  }
  
  Vector3 current_pos = owner_unit->get_global_position();
  Vector3 target_pos = owner_unit->get_desired_location();
  Vector3 direction = (target_pos - current_pos).normalized();
  
  // Debug: Log movement data every few frames
  static int frame_count = 0;
  if (++frame_count % 30 == 0) {  // Every 30 frames (~0.5 seconds at 60 FPS)
    DBG_VECTOR3("Movement", "current_position", current_pos);
    DBG_VECTOR3("Movement", "target_position", target_pos);
    DBG_VALUE("Movement", "distance_to_target", current_pos.distance_to(target_pos));
  }
  
  // Apply movement
  Vector3 velocity = direction * speed;
  owner_unit->set_velocity(velocity);
  owner_unit->move_and_slide();
  
  // Visual debug: Draw movement vector
  auto* debugger = VisualDebugger::get_singleton();
  if (debugger && debugger->is_debug_enabled()) {
    debugger->draw_vector(current_pos, velocity, 2.0f, 
                         Color(0, 1, 0, 1), 1.5f);
    debugger->draw_sphere(target_pos, 0.3f, 
                         Color(1, 0, 0, 0.5f), 8, 1.0f);
  }
}

void MovementComponent::set_speed(float new_speed) {
  if (new_speed < 0) {
    DBG_ERROR("Movement", "Speed cannot be negative: " + String::num(new_speed));
    return;
  }
  
  if (new_speed != speed) {
    DBG_VALUE("Movement", "speed_changed_to", new_speed);
    speed = new_speed;
  }
}
```

## Example 3: Combat System with Detailed Logging

```cpp
// attack_component.cpp
#include "attack_component.hpp"
#include "debug/debug_macros.hpp"
#include "debug/debug_utils.hpp"

void AttackComponent::_ready() {
  DBG_INFO("Combat", "Attack component initialized");
  owner_unit = Object::cast_to<Unit>(get_parent());
  
  if (!owner_unit) {
    DBG_ERROR("Combat", "Parent is not a Unit");
    return;
  }
  
  DBG_VALUE("Combat", "attack_range", attack_range);
  DBG_VALUE("Combat", "attack_damage", attack_damage);
  DBG_VALUE("Combat", "attack_cooldown", attack_cooldown);
}

bool AttackComponent::can_attack_target(Unit* target) {
  if (!target) {
    DBG_WARN("Combat", "Attempted to check attack on null target");
    return false;
  }
  
  // Check range
  Vector3 distance_vec = target->get_global_position() - 
                         owner_unit->get_global_position();
  float distance = distance_vec.length();
  
  if (distance > attack_range) {
    DBG_DEBUG("Combat", "Target out of range (" + String::num(distance, 2) + 
              "m vs " + String::num(attack_range, 2) + "m)");
    return false;
  }
  
  // Check cooldown
  if (is_on_cooldown) {
    float remaining = cooldown_timer;
    DBG_DEBUG("Combat", "Attack on cooldown (" + String::num(remaining, 2) + "s remaining)");
    return false;
  }
  
  // Check health
  auto* health = owner_unit->get_component<HealthComponent>("HealthComponent");
  if (!health || health->is_dead()) {
    DBG_ERROR("Combat", "Cannot attack while dead");
    return false;
  }
  
  DBG_INFO("Combat", owner_unit->get_name() + " can attack " + target->get_name());
  return true;
}

void AttackComponent::attack(Unit* target) {
  if (!can_attack_target(target)) {
    return;
  }
  
  DBG_INFO("Combat", owner_unit->get_name() + " attacking " + target->get_name());
  
  // Get health component
  auto* health = target->get_component<HealthComponent>("HealthComponent");
  if (!health) {
    DBG_ERROR("Combat", "Target has no health component");
    return;
  }
  
  // Calculate damage
  float damage = calculate_damage();
  DBG_VALUE("Combat", "damage_dealt", damage);
  
  // Apply damage
  health->take_damage(damage);
  DBG_VALUE("Combat", "target_hp_remaining", health->get_current_health());
  
  // Start cooldown
  is_on_cooldown = true;
  cooldown_timer = attack_cooldown;
  
  // Visual feedback
  auto* debugger = VisualDebugger::get_singleton();
  if (debugger && debugger->is_debug_enabled()) {
    Vector3 start_pos = owner_unit->get_global_position();
    Vector3 end_pos = target->get_global_position();
    debugger->draw_line(start_pos, end_pos, Color(1, 0, 0, 1), 3.0f);
    debugger->draw_cross(end_pos, 0.5f, Color(1, 0, 0, 1), 2.0f);
  }
}

void AttackComponent::_physics_process(double delta) {
  // Update cooldown
  if (is_on_cooldown) {
    cooldown_timer -= delta;
    if (cooldown_timer <= 0) {
      is_on_cooldown = false;
      DBG_DEBUG("Combat", "Attack cooldown expired");
    }
  }
  
  // Visual: Draw attack range
  auto* debugger = VisualDebugger::get_singleton();
  if (debugger && debugger->is_debug_enabled()) {
    Vector3 unit_pos = owner_unit->get_global_position();
    
    // Attack range (red)
    debugger->draw_circle_xz(unit_pos, attack_range, 
                            Color(1, 0, 0, 0.2f), 32, 1.0f, false);
    
    // Vision range (cyan, larger)
    debugger->draw_circle_xz(unit_pos, vision_range, 
                            Color(0, 1, 1, 0.1f), 32, 1.0f, false);
  }
}
```

## Example 4: Pathfinding with Visualization

```cpp
// pathfinding_system.cpp
#include "pathfinding_system.hpp"
#include "debug/debug_macros.hpp"
#include "debug/debug_utils.hpp"

std::vector<Vector3> PathfindingSystem::find_path(const Vector3& start, 
                                                   const Vector3& end) {
  DBG_INFO("Pathfinding", "Finding path from " + String::num(start.x) + 
           ", " + String::num(start.z));
  
  std::vector<Vector3> path;
  
  // Perform A* pathfinding
  // ... (pathfinding algorithm) ...
  
  if (path.empty()) {
    DBG_WARN("Pathfinding", "No valid path found");
    return path;
  }
  
  DBG_INFO("Pathfinding", "Found path with " + String::num(path.size()) + 
           " waypoints");
  
  // Visualize path
  auto* debugger = VisualDebugger::get_singleton();
  if (debugger && debugger->is_debug_enabled()) {
    Color green = Color(0, 1, 0, 1);
    
    // Draw waypoints
    for (size_t i = 0; i < path.size(); ++i) {
      float brightness = 1.0f - (i / (float)path.size());
      Color waypoint_color = Color(brightness, brightness, 0, 1);
      
      debugger->draw_sphere(path[i], 0.2f, waypoint_color, 8, 1.0f);
      
      if (i < path.size() - 1) {
        debugger->draw_line(path[i], path[i + 1], green, 2.0f);
      }
    }
    
    // Draw start and end markers
    debugger->draw_cross(start, 0.5f, Color(0, 1, 0, 1), 2.0f);  // Green = start
    debugger->draw_cross(end, 0.5f, Color(1, 0, 0, 1), 2.0f);     // Red = end
  }
  
  return path;
}
```

## Example 5: Ability System with Category Logging

```cpp
// ability_component.cpp
#include "ability_component.hpp"
#include "debug/debug_macros.hpp"

void AbilityComponent::_ready() {
  DBG_INFO("Abilities", "Ability component initialized");
  owner_unit = Object::cast_to<Unit>(get_parent());
  
  for (const auto& ability_id : ability_ids) {
    DBG_DEBUG("Abilities", "Loaded ability: " + ability_id);
  }
}

bool AbilityComponent::can_cast_ability(const String& ability_id) {
  if (!has_ability(ability_id)) {
    DBG_WARN("Abilities", "Ability not found: " + ability_id);
    return false;
  }
  
  auto* ability = get_ability(ability_id);
  
  // Check mana
  if (current_mana < ability->get_mana_cost()) {
    DBG_DEBUG("Abilities", "Insufficient mana for " + ability_id + 
              " (" + String::num(current_mana) + "/" + 
              String::num(ability->get_mana_cost()) + ")");
    return false;
  }
  
  // Check cooldown
  if (ability->is_on_cooldown()) {
    DBG_DEBUG("Abilities", ability_id + " is on cooldown");
    return false;
  }
  
  DBG_DEBUG("Abilities", "Can cast " + ability_id);
  return true;
}

void AbilityComponent::cast_ability(const String& ability_id, 
                                   Unit* target) {
  if (!can_cast_ability(ability_id)) {
    DBG_WARN("Abilities", "Cannot cast " + ability_id);
    return;
  }
  
  auto* ability = get_ability(ability_id);
  
  DBG_INFO("Abilities", owner_unit->get_name() + " casting " + ability_id);
  
  // Consume mana
  float mana_cost = ability->get_mana_cost();
  current_mana -= mana_cost;
  DBG_VALUE("Abilities", "mana_remaining", current_mana);
  
  // Execute ability
  ability->execute(target);
  
  // Start cooldown
  ability->start_cooldown();
  DBG_DEBUG("Abilities", ability_id + " on cooldown for " + 
            String::num(ability->get_cooldown(), 1) + "s");
}
```

## Example 6: Assertions for Debugging

```cpp
// unit.cpp
#include "unit.hpp"
#include "debug/debug_macros.hpp"

void Unit::set_health(float new_health) {
  // Validate input
  DBG_ASSERT(new_health >= 0, "Health", "Health cannot be negative");
  
  if (new_health < 0) {
    new_health = 0;  // Clamp
  }
  
  current_health = new_health;
  
  if (current_health <= 0) {
    DBG_INFO("Unit", get_name() + " died");
    die();
  }
}

void Unit::move_to(const Vector3& target) {
  // Ensure unit is initialized
  DBG_ASSERT(is_inside_tree(), "Movement", 
             "Unit must be in scene tree to move");
  
  if (!is_inside_tree()) {
    DBG_ERROR("Movement", get_name() + " is not in scene tree");
    return;
  }
  
  // Ensure target is valid
  DBG_ASSERT(target.is_finite(), "Movement", 
             "Target position contains NaN values");
  
  desired_location = target;
  DBG_DEBUG("Movement", "Moving to " + String::num(target.x) + 
            ", " + String::num(target.z));
}
```

## Example 7: Performance Monitoring

```cpp
// combat_system.cpp
#include "combat_system.hpp"
#include "debug/debug_macros.hpp"

void CombatSystem::process_attacks(double delta) {
  DBG_DEBUG("Combat", "Processing attacks for " + 
            String::num(active_units.size()) + " units");
  
  int attacks_performed = 0;
  int damage_dealt = 0;
  
  for (auto* unit : active_units) {
    auto* attack_component = unit->get_component<AttackComponent>();
    if (!attack_component) continue;
    
    auto* target = find_nearest_enemy(unit);
    if (attack_component->can_attack_target(target)) {
      attack_component->attack(target);
      attacks_performed++;
      
      // Track damage (would need to expose from component)
      // damage_dealt += last_damage_amount;
    }
  }
  
  // Log summary every second
  static float log_timer = 0;
  log_timer += delta;
  if (log_timer >= 1.0f) {
    DBG_INFO("Combat", "Attacks: " + String::num(attacks_performed) + 
             " | Damage: " + String::num(damage_dealt));
    log_timer = 0;
  }
}
```

## Example 8: Quick Toggle Script

Create this as `debug_toggle.gd` and attach to any node to enable quick debugging:

```gdscript
# debug_toggle.gd
# Toggle debugging features with keyboard shortcuts
extends Node

func _input(event: InputEvent) -> void:
  if not event is InputEventKey or not event.pressed:
    return
  
  match event.keycode:
    # D - Toggle visual debugging
    KEY_D:
      var debugger = VisualDebugger.get_singleton()
      if debugger:
        var was_enabled = debugger.is_debug_enabled()
        debugger.set_debug_enabled(!was_enabled)
        print("[DEBUG] Visual debugging: %s" % ("ENABLED" if not was_enabled else "DISABLED"))
    
    # L - Cycle log levels
    KEY_L:
      var logger = DebugLogger.get_singleton()
      if logger:
        var levels = ["DEBUG", "INFO", "WARNING", "ERROR"]
        var current = logger.get_log_level()
        var next = (current + 1) % 4
        logger.set_log_level(next)
        print("[DEBUG] Log level set to: %s" % levels[next])
    
    # M - Print memory stats
    KEY_M:
      var mem_static = OS.get_static_memory_usage()
      var mem_dynamic = OS.get_dynamic_memory_usage()
      print("[DEBUG] Memory - Static: %.2f MB, Dynamic: %.2f MB" % 
            [mem_static / 1024.0 / 1024.0, mem_dynamic / 1024.0 / 1024.0])
    
    # P - Print active nodes count
    KEY_P:
      var root = get_tree().root
      print("[DEBUG] Active nodes in tree: %d" % root.get_child_count())
```

## Using These Examples

1. **Copy** the relevant sections
2. **Adapt** to your specific components
3. **Use consistent categories** (Movement, Combat, Abilities, Physics, etc.)
4. **Start with DBG_INFO** and add DBG_DEBUG as needed
5. **Add visual debugging** for spatial systems
6. **Test** by creating a DebugLogger instance in _ready()

All examples follow these principles:
- Always check for null pointers before logging
- Use meaningful category names
- Log entry and exit of important functions
- Log state changes and validation failures
- Add visual feedback for spatial operations
