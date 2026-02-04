#ifndef GDEXTENSION_UNIT_H
#define GDEXTENSION_UNIT_H

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include "../common/unit_order.hpp"

namespace godot {
class Object;
class Node;
class StringName;
}  // namespace godot

using godot::CharacterBody3D;
using godot::PackedStringArray;
using godot::String;
using godot::StringName;
using godot::Vector3;

class Interactable;
class HealthComponent;
class AttackComponent;
class MovementComponent;
class AbilityComponent;
class LabelRegistry;

/// Character entity with components - pure signal dispatcher
/// The playable/NPC unit that receives orders and emits signals
///
/// Architecture:
/// - Unit is a CharacterBody3D container for components
/// - Does NOT contain game logic - all logic lives in components
/// - Receives orders from InputManager or AI
/// - Emits signals with order details, then forgets about them (fire and
/// forget)
/// - Components independently listen to signals and decide what to do
/// - Unit doesn't care if components exist or are listening
///
/// Component Pattern:
/// - MovementComponent: Listens to orders, handles movement and rotation
/// - AttackComponent: Listens to orders, handles attack timing and projectiles
/// - AbilityComponent: Listens to orders, handles ability casting and cooldowns
/// - HealthComponent: Tracks HP and death
///
/// To Use:
/// 1. Add Unit to scene
/// 2. Add desired components as children
/// 3. Components auto-wire and listen to signals via _ready()
/// 4. InputManager/AI calls issue_move_order(), issue_attack_order()
/// 5. Unit emits signals, components independently respond
class Unit : public CharacterBody3D {
  GDCLASS(Unit, CharacterBody3D)

 protected:
  static void _bind_methods();

 public:
  Unit();
  ~Unit();

  void _ready() override;

  // Order methods - emit signals for components to listen to
  // Fire and forget: Unit doesn't care if components exist or respond
  void issue_move_order(const Vector3& position);
  void issue_attack_order(Unit* target);
  void issue_chase_order(Unit* target);
  void issue_interact_order(Interactable* target);
  void stop_order();

  // Component lookup helpers (for components to find each other)
  godot::Node* get_component_by_class(const StringName& class_name) const;
  HealthComponent* get_health_component() const;
  AttackComponent* get_attack_component() const;
  AbilityComponent* get_ability_component() const;

  // Faction for team identification
  void set_faction_id(int32_t new_faction_id);
  int32_t get_faction_id() const;

  // Display name (for UI/debug purposes, not unique)
  void set_unit_name(const String& name);
  String get_unit_name() const;

  // Debug label registration - called by LabelComponent
  void register_all_debug_labels(LabelRegistry* registry);

 private:
  int32_t faction_id = 0;
  String unit_name = "Unit";
};

#endif  // GDEXTENSION_UNIT_H
