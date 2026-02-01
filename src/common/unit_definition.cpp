#include "unit_definition.hpp"

#include <algorithm>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../components/abilities/ability_definition.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

UnitDefinition::UnitDefinition() {
  // Initialize ability array with default count of 4 slots
  // User can resize via inspector "Add Element" button
  abilities.resize(4);
}

UnitDefinition::~UnitDefinition() = default;

void UnitDefinition::_bind_methods() {
  // Unit identity properties
  ClassDB::bind_method(D_METHOD("set_unit_name", "name"),
                       &UnitDefinition::set_unit_name);
  ClassDB::bind_method(D_METHOD("get_unit_name"),
                       &UnitDefinition::get_unit_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "unit_name"), "set_unit_name",
               "get_unit_name");

  ClassDB::bind_method(D_METHOD("set_unit_id", "id"),
                       &UnitDefinition::set_unit_id);
  ClassDB::bind_method(D_METHOD("get_unit_id"), &UnitDefinition::get_unit_id);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "unit_id"), "set_unit_id",
               "get_unit_id");

  ClassDB::bind_method(D_METHOD("set_description", "description"),
                       &UnitDefinition::set_description);
  ClassDB::bind_method(D_METHOD("get_description"),
                       &UnitDefinition::get_description);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description",
               "get_description");

  ClassDB::bind_method(D_METHOD("set_unit_type", "type"),
                       &UnitDefinition::set_unit_type);
  ClassDB::bind_method(D_METHOD("get_unit_type"),
                       &UnitDefinition::get_unit_type);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "unit_type"), "set_unit_type",
               "get_unit_type");

  // Ability slots (Q, W, E, R, D, F - configurable count via inspector)
  ClassDB::bind_method(D_METHOD("set_ability", "slot", "ability"),
                       &UnitDefinition::set_ability);
  ClassDB::bind_method(D_METHOD("get_ability", "slot"),
                       &UnitDefinition::get_ability);
  ClassDB::bind_method(D_METHOD("has_ability", "slot"),
                       &UnitDefinition::has_ability);
  ClassDB::bind_method(D_METHOD("get_abilities"),
                       &UnitDefinition::get_abilities);
  ClassDB::bind_method(D_METHOD("set_abilities", "abilities"),
                       &UnitDefinition::set_abilities);

  // Expose abilities array for editor editing
  ADD_PROPERTY(
      PropertyInfo(Variant::ARRAY, "abilities", godot::PROPERTY_HINT_ARRAY_TYPE,
                   "AbilityDefinition"),
      "set_abilities", "get_abilities");
}

void UnitDefinition::set_unit_name(const String& name) {
  unit_name = name;
}

String UnitDefinition::get_unit_name() const {
  return unit_name;
}

void UnitDefinition::set_unit_id(int id) {
  unit_id = id;
}

int UnitDefinition::get_unit_id() const {
  return unit_id;
}

void UnitDefinition::set_description(const String& desc) {
  description = desc;
}

String UnitDefinition::get_description() const {
  return description;
}

void UnitDefinition::set_unit_type(const String& type) {
  unit_type = type;
}

String UnitDefinition::get_unit_type() const {
  return unit_type;
}

void UnitDefinition::set_ability(int slot,
                                 const Ref<AbilityDefinition>& ability) {
  if (slot < 0 || slot >= static_cast<int>(abilities.size())) {
    UtilityFunctions::print("[UnitDefinition] Invalid ability slot: " +
                            godot::String::num(slot));
    return;
  }
  abilities[slot] = ability;
}

Ref<AbilityDefinition> UnitDefinition::get_ability(int slot) const {
  if (slot < 0 || slot >= static_cast<int>(abilities.size())) {
    return nullptr;
  }
  Ref<AbilityDefinition> ability = abilities[slot];
  return ability;
}

bool UnitDefinition::has_ability(int slot) const {
  if (slot < 0 || slot >= static_cast<int>(abilities.size())) {
    return false;
  }
  Ref<AbilityDefinition> ability = abilities[slot];
  return ability.is_valid();
}

Array UnitDefinition::get_abilities() const {
  return abilities;
}

void UnitDefinition::set_abilities(const Array& new_abilities) {
  abilities = new_abilities;
  // Ensure array matches static_cast<int>(abilities.size())
  if (abilities.size() != static_cast<int>(abilities.size())) {
    abilities.resize(static_cast<int>(abilities.size()));
  }
}
