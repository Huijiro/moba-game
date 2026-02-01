#ifndef GDEXTENSION_ABILITY_COMPONENT_H
#define GDEXTENSION_ABILITY_COMPONENT_H

#include <godot_cpp/classes/ref.hpp>
#include <vector>

#include "../unit_component.hpp"
#include "ability_definition.hpp"
#include "ability_types.hpp"

using godot::Object;
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
/// 2. Auto-populate from UnitDefinition via Unit._ready()
/// 3. Call try_cast(slot, target) or try_cast_point(slot, point)
/// 4. Query states: is_casting(), is_on_cooldown(), get_cooldown_remaining()
class AbilityComponent : public UnitComponent {
  GDCLASS(AbilityComponent, UnitComponent)

 protected:
  static void _bind_methods();

  // Ability slots (configurable, typically 4-6: Q, W, E, R, D, F)
  std::vector<Ref<AbilityDefinition>> ability_slots;

  // Cooldown tracking per ability slot
  std::vector<float> cooldown_timers;

  // Casting state
  int casting_slot = -1;
  Object* casting_target = nullptr;
  Vector3 casting_point = Vector3(0, 0, 0);
  float casting_timer = 0.0f;
  int casting_state = static_cast<int>(CastState::IDLE);

  // Resource pool reference (for mana checks)
  ResourcePoolComponent* resource_pool = nullptr;

 public:
  AbilityComponent();
  ~AbilityComponent();

  void _physics_process(double delta) override;
  void _ready() override;

  // ========== ABILITY SLOT MANAGEMENT ==========
  void set_ability(int slot, const Ref<AbilityDefinition>& ability);
  Ref<AbilityDefinition> get_ability(int slot);
  bool has_ability(int slot);

  int get_ability_count() const;
  void set_ability_count(int count);

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
