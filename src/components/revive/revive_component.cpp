#include "revive_component.hpp"

#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../common/unit_signals.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../health/health_component.hpp"
#include "../ui/label_registry.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::StringName;
using godot::UtilityFunctions;
using godot::Variant;

ReviveComponent::ReviveComponent() = default;

ReviveComponent::~ReviveComponent() = default;

void ReviveComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_revive_time", "time"),
                       &ReviveComponent::set_revive_time);
  ClassDB::bind_method(D_METHOD("get_revive_time"),
                       &ReviveComponent::get_revive_time);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "revive_time"), "set_revive_time",
               "get_revive_time");

  ClassDB::bind_method(D_METHOD("start_revive"),
                       &ReviveComponent::start_revive);

  // Signal handler for death
  ClassDB::bind_method(D_METHOD("_on_unit_died", "source"),
                       &ReviveComponent::_on_unit_died);
}

void ReviveComponent::_ready() {
  UnitComponent::_ready();

  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    return;
  }

  // Find the health component
  for (int i = 0; i < owner->get_child_count(); ++i) {
    godot::Node* child = owner->get_child(i);
    if (child == nullptr)
      continue;
    health_component = godot::Object::cast_to<HealthComponent>(child);
    if (health_component != nullptr) {
      // Connect to the health component's died signal
      health_component->connect(
          StringName("died"),
          godot::Callable(this, StringName("_on_unit_died")));
      break;
    }
  }

  if (health_component == nullptr) {
    UtilityFunctions::push_error(
        "[ReviveComponent] No HealthComponent found in unit children");
  }
}

void ReviveComponent::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Handle revive countdown
  if (is_reviving) {
    revive_timer -= delta;

    if (revive_timer <= 0.0) {
      // Revive the unit
      is_reviving = false;

      Unit* owner = get_unit();
      if (owner != nullptr && health_component != nullptr) {
        // Restore full health
        health_component->set_current_health(
            health_component->get_max_health());

        // Re-enable collision
        _enable_collision();

        DBG_INFO("ReviveComponent", "" + owner->get_name() + " has revived!");
      }
    }
  }
}

void ReviveComponent::set_revive_time(float time) {
  revive_time = time > 0.0f ? time : 0.1f;
}

float ReviveComponent::get_revive_time() const {
  return revive_time;
}

void ReviveComponent::start_revive() {
  Unit* owner = get_unit();
  if (owner != nullptr) {
    DBG_INFO("ReviveComponent",
             "" + owner->get_name() + " started reviving...");
  }

  is_reviving = true;
  revive_timer = revive_time;
}

void ReviveComponent::_on_unit_died(godot::Object* source) {
  // Automatically start revive when unit dies
  start_revive();
}

void ReviveComponent::_enable_collision() {
  Unit* owner = get_unit();
  if (owner == nullptr || !owner->is_inside_tree()) {
    return;
  }

  // Find and enable all CollisionShape3D children of the owner
  for (int i = 0; i < owner->get_child_count(); i++) {
    godot::Node* child = owner->get_child(i);
    godot::CollisionShape3D* collision =
        godot::Object::cast_to<godot::CollisionShape3D>(child);
    if (collision != nullptr) {
      collision->set_disabled(false);
    }
  }

  // Also enable the CharacterBody3D collision layers
  owner->set_collision_layer(1);
  owner->set_collision_mask(1);

  DBG_INFO("ReviveComponent", "Enabled collision for " + owner->get_name());
}

void ReviveComponent::register_debug_labels(LabelRegistry* registry) {
  if (!registry) {
    return;
  }

  registry->register_property("Revive", "status",
                              is_reviving ? "REVIVING" : "ALIVE");
  if (is_reviving) {
    registry->register_property("Revive", "timer",
                                godot::String::num(revive_timer, 1));
  }
}
