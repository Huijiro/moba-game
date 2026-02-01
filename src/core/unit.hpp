#ifndef GDEXTENSION_UNIT_H
#define GDEXTENSION_UNIT_H

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include "../common/unit_definition.hpp"
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

class Unit : public CharacterBody3D {
  GDCLASS(Unit, CharacterBody3D)

 protected:
  static void _bind_methods();

 public:
  Unit();
  ~Unit();

  void _ready() override;
  void _physics_process(double delta) override;

  void issue_move_order(const Vector3& position);
  void issue_attack_order(Unit* target);
  void issue_interact_order(Interactable* target);
  void stop_order();

  void set_desired_location(const Vector3& location);
  Vector3 get_desired_location() const;

  void set_auto_attack_range(float new_range);
  float get_auto_attack_range() const;

  void set_attack_buffer_range(float new_buffer);
  float get_attack_buffer_range() const;

  void set_faction_id(int32_t new_faction_id);
  int32_t get_faction_id() const;

  // Component lookup helpers
  godot::Node* get_component_by_class(const StringName& class_name) const;
  HealthComponent* get_health_component() const;
  AttackComponent* get_attack_component() const;
  AbilityComponent* get_ability_component() const;

  // Unit definition setup
  void set_unit_definition(const godot::Ref<UnitDefinition>& unit_def);
  godot::Ref<UnitDefinition> get_unit_definition() const;

 private:
  void _set_order(OrderType new_order, godot::Object* new_target);
  void _clear_order_targets();

  Vector3 desired_location = Vector3(0, 0, 0);

  OrderType current_order = OrderType::NONE;
  godot::Object* current_order_target = nullptr;
  Unit* attack_target = nullptr;
  Interactable* interact_target = nullptr;

  float auto_attack_range = 2.5f;
  float attack_buffer_range = 0.5f;  // Hysteresis buffer for resuming chase
  int32_t faction_id = 0;

  MovementComponent* movement_component = nullptr;
  AbilityComponent* ability_component = nullptr;
  godot::Ref<UnitDefinition> unit_definition = nullptr;
};

#endif  // GDEXTENSION_UNIT_H
