#include "resource_pool_component.hpp"

#include <algorithm>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include "../../common/unit_signals.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::String;
using godot::UtilityFunctions;
using godot::Variant;

ResourcePoolComponent::ResourcePoolComponent() = default;

ResourcePoolComponent::~ResourcePoolComponent() = default;

void ResourcePoolComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_pool_id", "id"),
                       &ResourcePoolComponent::set_pool_id);
  ClassDB::bind_method(D_METHOD("get_pool_id"),
                       &ResourcePoolComponent::get_pool_id);
  ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "pool_id"), "set_pool_id",
               "get_pool_id");

  ClassDB::bind_method(D_METHOD("set_max_value", "value"),
                       &ResourcePoolComponent::set_max_value);
  ClassDB::bind_method(D_METHOD("get_max_value"),
                       &ResourcePoolComponent::get_max_value);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_value"), "set_max_value",
               "get_max_value");

  ClassDB::bind_method(D_METHOD("set_current_value", "value"),
                       &ResourcePoolComponent::set_current_value);
  ClassDB::bind_method(D_METHOD("get_current_value"),
                       &ResourcePoolComponent::get_current_value);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_value"),
               "set_current_value", "get_current_value");

  ClassDB::bind_method(D_METHOD("can_spend", "amount"),
                       &ResourcePoolComponent::can_spend);
  ClassDB::bind_method(D_METHOD("try_spend", "amount"),
                       &ResourcePoolComponent::try_spend);
  ClassDB::bind_method(D_METHOD("restore", "amount"),
                       &ResourcePoolComponent::restore);

  // Bind signal handler for resource pool queries
  ClassDB::bind_method(
      D_METHOD("_on_resource_pool_requested", "requested_pool_id"),
      &ResourcePoolComponent::_on_resource_pool_requested);

  ADD_SIGNAL(godot::MethodInfo("value_changed",
                               PropertyInfo(Variant::FLOAT, "current"),
                               PropertyInfo(Variant::FLOAT, "max")));
}

void ResourcePoolComponent::set_pool_id(StringName id) {
  pool_id = id;
}

StringName ResourcePoolComponent::get_pool_id() const {
  return pool_id;
}

void ResourcePoolComponent::set_max_value(float value) {
  float old_max = max_value;
  max_value = std::max(0.0f, value);
  if (current_value > max_value) {
    current_value = max_value;
  }
  DBG_DEBUG("ResourcePool", String(pool_id) +
                                " max changed: " + String::num(old_max) +
                                " -> " + String::num(max_value));
  emit_signal("value_changed", current_value, max_value);
}

float ResourcePoolComponent::get_max_value() const {
  return max_value;
}

void ResourcePoolComponent::set_current_value(float value) {
  float old_value = current_value;
  current_value = std::clamp(value, 0.0f, max_value);
  if (old_value != current_value) {
    DBG_DEBUG("ResourcePool", String(pool_id) +
                                  " value changed: " + String::num(old_value) +
                                  " -> " + String::num(current_value) + " / " +
                                  String::num(max_value));
  }
  emit_signal("value_changed", current_value, max_value);
}

float ResourcePoolComponent::get_current_value() const {
  return current_value;
}

bool ResourcePoolComponent::can_spend(float amount) const {
  return current_value >= amount && amount >= 0.0f;
}

bool ResourcePoolComponent::try_spend(float amount) {
  if (!can_spend(amount)) {
    return false;
  }

  float old_value = current_value;
  current_value -= amount;
  DBG_DEBUG("ResourcePool", String(pool_id) + " spent " + String::num(amount) +
                                ": " + String::num(old_value) + " -> " +
                                String::num(current_value) + " / " +
                                String::num(max_value));
  emit_signal("value_changed", current_value, max_value);
  return true;
}

void ResourcePoolComponent::restore(float amount) {
  if (amount < 0.0f) {
    amount = 0.0f;
  }

  float old_value = current_value;
  current_value = std::min(max_value, current_value + amount);
  DBG_DEBUG("ResourcePool",
            String(pool_id) + " restored " + String::num(amount) + ": " +
                String::num(old_value) + " -> " + String::num(current_value) +
                " / " + String::num(max_value));
  emit_signal("value_changed", current_value, max_value);
}

void ResourcePoolComponent::_ready() {
  UnitComponent::_ready();

  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    return;
  }

  // Register and connect to resource pool query signal
  owner->register_signal(resource_pool_requested);
  owner->connect(
      resource_pool_requested,
      godot::Callable(this, godot::StringName("_on_resource_pool_requested")));
}

void ResourcePoolComponent::_on_resource_pool_requested(
    const godot::StringName& requested_pool_id) {
  // If this pool matches the requested pool_id, provide our reference
  if (requested_pool_id == pool_id) {
    Unit* owner = get_unit();
    if (owner != nullptr) {
      // Emit the response through the unit relay
      owner->relay(resource_pool_requested, godot::Variant(this));
    }
  }
}
