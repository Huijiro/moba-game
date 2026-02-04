#include "health_component.hpp"

#include <algorithm>
#include <godot_cpp/classes/collision_shape3d.hpp>
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
#include "../ui/label_registry.hpp"

using godot::Callable;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::StringName;
using godot::UtilityFunctions;
using godot::Variant;

HealthComponent::HealthComponent() = default;

HealthComponent::~HealthComponent() = default;

void HealthComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_max_health", "value"),
                       &HealthComponent::set_max_health);
  ClassDB::bind_method(D_METHOD("get_max_health"),
                       &HealthComponent::get_max_health);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_health"), "set_max_health",
               "get_max_health");

  ClassDB::bind_method(D_METHOD("set_current_health", "value"),
                       &HealthComponent::set_current_health);
  ClassDB::bind_method(D_METHOD("get_current_health"),
                       &HealthComponent::get_current_health);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current_health"),
               "set_current_health", "get_current_health");

  ClassDB::bind_method(D_METHOD("apply_damage", "amount", "source"),
                       &HealthComponent::apply_damage, nullptr);
  ClassDB::bind_method(D_METHOD("heal", "amount"), &HealthComponent::heal);
  ClassDB::bind_method(D_METHOD("is_dead"), &HealthComponent::is_dead);

  // Signal handler for take_damage relay
  ClassDB::bind_method(D_METHOD("_on_take_damage", "damage", "source"),
                       &HealthComponent::_on_take_damage);

  ADD_SIGNAL(godot::MethodInfo("health_changed",
                               PropertyInfo(Variant::FLOAT, "current"),
                               PropertyInfo(Variant::FLOAT, "max")));
  ADD_SIGNAL(
      godot::MethodInfo("died", PropertyInfo(Variant::OBJECT, "source")));
}

void HealthComponent::_ready() {
  UnitComponent::_ready();

  if (godot::Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    return;
  }

  // Connect to Unit's take_damage signal using dynamic signal relay
  owner->connect_signal(
      take_damage, godot::Callable(this, godot::StringName("_on_take_damage")));
}

void HealthComponent::set_max_health(float value) {
  max_health = std::max(0.0f, value);
  if (current_health > max_health) {
    current_health = max_health;
  }
  emit_signal("health_changed", current_health, max_health);
}

float HealthComponent::get_max_health() const {
  return max_health;
}

void HealthComponent::set_current_health(float value) {
  current_health = std::clamp(value, 0.0f, max_health);
  emit_signal("health_changed", current_health, max_health);

  if (current_health <= 0.0f) {
    is_dead_flag = true;
    _disable_collision();
    emit_signal("died", nullptr);
  }
}

float HealthComponent::get_current_health() const {
  return current_health;
}

bool HealthComponent::apply_damage(float amount, godot::Object* source) {
  if (amount < 0.0f) {
    amount = 0.0f;
  }

  current_health = std::max(0.0f, current_health - amount);
  emit_signal("health_changed", current_health, max_health);

  // Log damage
  if (owner_unit != nullptr) {
    DBG_INFO("HealthComponent",
             "" + owner_unit->get_name() + " took " +
                 godot::String::num(amount) +
                 " damage. HP: " + godot::String::num(current_health) + "/" +
                 godot::String::num(max_health));
  } else {
    DBG_INFO("HealthComponent",
             "Took " + godot::String::num(amount) +
                 " damage. HP: " + godot::String::num(current_health) + "/" +
                 godot::String::num(max_health));
  }

  if (current_health <= 0.0f) {
    if (owner_unit != nullptr) {
      DBG_INFO("HealthComponent", "" + owner_unit->get_name() + " died!");
    } else {
      DBG_INFO("HealthComponent", "Unit died!");
    }

    is_dead_flag = true;
    _disable_collision();
    emit_signal("died", source);
    return true;  // Unit died
  }

  return false;  // Unit survived
}

void HealthComponent::heal(float amount) {
  if (amount < 0.0f) {
    amount = 0.0f;
  }

  current_health = std::min(max_health, current_health + amount);
  emit_signal("health_changed", current_health, max_health);
}

bool HealthComponent::is_dead() const {
  return current_health <= 0.0f;
}

void HealthComponent::_disable_collision() {
  if (owner_unit == nullptr || !owner_unit->is_inside_tree()) {
    return;
  }

  // Find and disable all CollisionShape3D children of the owner
  for (int i = 0; i < owner_unit->get_child_count(); i++) {
    godot::Node* child = owner_unit->get_child(i);
    godot::CollisionShape3D* collision =
        godot::Object::cast_to<godot::CollisionShape3D>(child);
    if (collision != nullptr) {
      collision->set_disabled(true);
    }
  }

  // Also disable the CharacterBody3D collision layers
  owner_unit->set_collision_layer(0);
  owner_unit->set_collision_mask(0);

  DBG_INFO("HealthComponent",
           "Disabled collision for " + owner_unit->get_name());
}

void HealthComponent::_on_take_damage(float damage, godot::Object* source) {
  // Fire-and-forget: receive damage signal and apply it
  apply_damage(damage, source);
}

void HealthComponent::register_debug_labels(LabelRegistry* registry) {
  if (!registry) {
    return;
  }

  registry->register_property("Health", "current",
                              godot::String::num(current_health));
  registry->register_property("Health", "max", godot::String::num(max_health));
  registry->register_property("Health", "status",
                              is_dead_flag ? "DEAD" : "ALIVE");
}
