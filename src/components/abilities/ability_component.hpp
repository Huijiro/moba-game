#ifndef GDEXTENSION_ABILITY_COMPONENT_H
#define GDEXTENSION_ABILITY_COMPONENT_H

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <vector>

#include "../unit_component.hpp"
#include "ability_definition.hpp"
#include "ability_node.hpp"
#include "ability_types.hpp"

using godot::Object;
using godot::PackedScene;
using godot::Ref;
using godot::Vector3;

class ResourcePoolComponent;

/// Core ability system component
/// Manages ability slots, casting, cooldowns, and mana costs
/// Attaches to Unit nodes and provides ability execution interface
///
/// Documentation: See docs/ability-system/00_INDEX.md
/// - Core Components section for architecture overview
/// - Key Methods: try_cast(), try_cast_point(), is_casting()
/// - State machine: IDLE, CASTING, CHANNELING, ON_COOLDOWN
/// - Integrates with ResourcePoolComponent for mana validation
///
/// Usage:
/// 1. Add AbilityComponent as child of Unit
/// 2. Configure abilities array in the editor or via set_abilities()
/// 3. Call try_cast(slot, target) or try_cast_point(slot, point)
/// 4. Query states: is_casting(), is_on_cooldown(), get_cooldown_remaining()
class AbilityComponent : public UnitComponent {
  GDCLASS(AbilityComponent, UnitComponent)

 protected:
  static void _bind_methods();

  // Ability slots - stored as PackedScene references during editor
  // Only instantiated to AbilityNode instances at runtime (in _ready)
  godot::Array ability_scenes;  // Stores either PackedScene or AbilityNode

  // Cooldown tracking per ability slot
  std::vector<float> cooldown_timers;

  // Casting state
  int casting_slot = -1;
  Object* casting_target = nullptr;
  Vector3 casting_point = Vector3(0, 0, 0);
  float casting_timer = 0.0f;
  int casting_state = static_cast<int>(CastState::IDLE);

  // Channel ticking (for periodic damage abilities)
  float next_tick_time = 0.0f;  // When the next tick should occur

  // Resource pool reference (for mana checks)
  ResourcePoolComponent* resource_pool = nullptr;

 public:
  AbilityComponent();
  ~AbilityComponent();

  void _physics_process(double delta) override;
  void _ready() override;

  // ========== ABILITY SLOT MANAGEMENT ==========
  void set_ability_scene(int slot, const Ref<PackedScene>& scene);
  AbilityNode* get_ability(int slot);
  bool has_ability(int slot);

  int get_ability_count() const;
  void set_ability_count(int count);

  // Array-based interface for editor exposure (PackedScene array)
  void set_ability_scenes(const godot::Array& scenes);
  godot::Array get_ability_scenes() const;

  // Legacy compatibility - also bind as set_abilities/get_abilities
  void set_abilities(const godot::Array& scenes);
  godot::Array get_abilities() const;

  // ========== ABILITY CASTING (Main Entry Points) ==========
  // Try to cast ability at a unit target
  bool try_cast(int slot, Object* target);

  // Try to cast ability at a point (for point-target abilities)
  bool try_cast_point(int slot, const Vector3& point);

  // ========== STATE QUERIES ==========
  bool is_casting() const;
  bool is_on_cooldown(int slot) const;
  float get_cooldown_remaining(int slot) const;
  float get_cooldown_duration(int slot) const;
  int get_cast_state(int slot) const;

  // ========== CASTING CONTROL ==========
  // Interrupt active channel or cast (applies cooldown)
  void interrupt_casting();

  // ========== INTERNAL METHODS ==========
 private:
  // Get resource pool by ID, or return default if not found
  ResourcePoolComponent* _get_resource_pool(const godot::String& pool_id);

  // Validation: check if ability can be cast
  bool _can_cast(int slot);

  // Check if we have enough resources (mana)
  bool _can_afford(int slot);

  // Begin the casting phase
  void _begin_cast(int slot, Object* target);

  // Execute the ability (call effect->execute)
  void _execute_ability(int slot);

  // Apply cooldown after ability executes
  void _apply_cooldown(int slot);

  // Transition out of casting state
  void _finish_casting();
};

#endif  // GDEXTENSION_ABILITY_COMPONENT_H
