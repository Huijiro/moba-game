#include "unit_definition_manager.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "unit_definition.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::UtilityFunctions;

// Initialize static members
std::map<int, Ref<UnitDefinition>> UnitDefinitionManager::unit_registry;
UnitDefinitionManager* UnitDefinitionManager::singleton = nullptr;

UnitDefinitionManager::UnitDefinitionManager() {
  if (singleton == nullptr) {
    singleton = this;
  }
}

UnitDefinitionManager::~UnitDefinitionManager() {
  if (singleton == this) {
    singleton = nullptr;
  }
}

void UnitDefinitionManager::_bind_methods() {
  ClassDB::bind_static_method("UnitDefinitionManager",
                              D_METHOD("register_unit", "unit_def"),
                              &UnitDefinitionManager::register_unit);
  ClassDB::bind_static_method("UnitDefinitionManager",
                              D_METHOD("get_unit_by_id", "unit_id"),
                              &UnitDefinitionManager::get_unit_by_id);
  ClassDB::bind_static_method("UnitDefinitionManager",
                              D_METHOD("get_unit_by_name", "name"),
                              &UnitDefinitionManager::get_unit_by_name);
  ClassDB::bind_static_method("UnitDefinitionManager",
                              D_METHOD("has_unit", "unit_id"),
                              &UnitDefinitionManager::has_unit);
  ClassDB::bind_static_method("UnitDefinitionManager",
                              D_METHOD("list_all_units"),
                              &UnitDefinitionManager::list_all_units);
}

void UnitDefinitionManager::_ready() {
  UtilityFunctions::print("[UnitDefinitionManager] Initialized with " +
                          godot::String::num(unit_registry.size()) +
                          " registered units");
}

UnitDefinitionManager* UnitDefinitionManager::get_singleton() {
  return singleton;
}

void UnitDefinitionManager::register_unit(const Ref<UnitDefinition>& unit_def) {
  if (unit_def.is_null()) {
    UtilityFunctions::print(
        "[UnitDefinitionManager] Cannot register null unit definition");
    return;
  }

  int unit_id = unit_def->get_unit_id();
  if (unit_id < 0) {
    UtilityFunctions::print(
        "[UnitDefinitionManager] Cannot register unit with invalid ID: " +
        godot::String::num(unit_id));
    return;
  }

  unit_registry[unit_id] = unit_def;
  UtilityFunctions::print(
      "[UnitDefinitionManager] Registered unit: " + unit_def->get_unit_name() +
      " (ID: " + godot::String::num(unit_id) +
      ", Type: " + unit_def->get_unit_type() + ")");
}

Ref<UnitDefinition> UnitDefinitionManager::get_unit_by_id(int unit_id) {
  auto it = unit_registry.find(unit_id);
  if (it != unit_registry.end()) {
    return it->second;
  }
  return nullptr;
}

Ref<UnitDefinition> UnitDefinitionManager::get_unit_by_name(
    const String& name) {
  for (auto& pair : unit_registry) {
    if (pair.second->get_unit_name() == name) {
      return pair.second;
    }
  }
  return nullptr;
}

bool UnitDefinitionManager::has_unit(int unit_id) {
  return unit_registry.find(unit_id) != unit_registry.end();
}

void UnitDefinitionManager::list_all_units() {
  UtilityFunctions::print(
      "[UnitDefinitionManager] Listing all registered units:");
  for (auto& pair : unit_registry) {
    int unit_id = pair.first;
    Ref<UnitDefinition> unit_def = pair.second;
    UtilityFunctions::print("  - " + unit_def->get_unit_name() +
                            " (ID: " + godot::String::num(unit_id) +
                            ", Type: " + unit_def->get_unit_type() + ")");
  }
}
