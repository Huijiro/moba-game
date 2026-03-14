#ifndef GDEXTENSION_ABILITY_COMPONENT_H
#define GDEXTENSION_ABILITY_COMPONENT_H

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>

#include "../unit_component.hpp"
#include "ability_context.hpp"
#include "ability_node.hpp"

using godot::PackedScene;
using godot::Ref;
using godot::Vector3;

/// AbilityComponent — thin ability slot manager on Unit.
///
/// Responsibilities:
/// - Stores and instantiates AbilityNode scenes as children
/// - Routes cast requests through AbilityNode::can_cast / start_cast
/// - Listens to ability lifecycle signals for state tracking
/// - Handles chase-into-range retry logic
///
/// Does NOT manage:
/// - Cast timing (owned by CastTime/Instant/Channel components)
/// - Cooldowns (owned by CooldownComponent)
/// - Resource costs (owned by ResourceCostComponent)
/// - Damage (owned by DamageEffectComponent)
class AbilityComponent : public UnitComponent {
  GDCLASS(AbilityComponent, UnitComponent)

 protected:
  static void _bind_methods();

 public:
  AbilityComponent();
  ~AbilityComponent();

  void _ready() override;

  // ========== ABILITY SLOT MANAGEMENT ==========
  void set_ability_scenes(const godot::Array& scenes);
  godot::Array get_ability_scenes() const;
  AbilityNode* get_ability(int slot);
  bool has_ability(int slot);
  int get_ability_count() const;

  // ========== CASTING ==========
  bool try_cast(int slot, godot::Object* target);
  bool try_cast_point(int slot, const Vector3& point);
  bool is_casting() const;
  void interrupt_casting();

  // Debug
  void register_debug_labels(LabelRegistry* registry) override;

 private:
  godot::Array ability_scenes;  // PackedScene refs (editor) or AbilityNode* (runtime)

  // Active cast state
  int casting_slot = -1;
  Ref<AbilityContext> active_context;

  // Pending cast (for chase-into-range retry)
  int pending_slot = -1;
  godot::Object* pending_target = nullptr;
  Vector3 pending_point = Vector3();

  // Instantiate ability scenes and add as children
  void _instantiate_abilities();

  // Connect to an ability's lifecycle signals
  void _connect_ability_signals(AbilityNode* ability, int slot);

  // Signal handlers
  void _on_completed(const Ref<AbilityContext>& context);
  void _on_cooldown_started(const Ref<AbilityContext>& context);
  void _on_cooldown_finished(const Ref<AbilityContext>& context);
  void _on_chase_range_reached(godot::Object* target);

  // Input signal handlers
  void _on_cast_ability(int slot, godot::Object* target,
                       const Vector3& position);
};

#endif  // GDEXTENSION_ABILITY_COMPONENT_H
