#include "attack_component.hpp"

#include <algorithm>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../core/unit.hpp"
#include "../health/health_component.hpp"
#include "projectile.hpp"
#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::String;
using godot::UtilityFunctions;
using godot::Variant;

AttackComponent::AttackComponent() = default;

AttackComponent::~AttackComponent() = default;

void AttackComponent::_bind_methods() {
  // Signal handler
  ClassDB::bind_method(D_METHOD("_on_unit_order_changed", "previous_order",
                                "new_order", "target"),
                       &AttackComponent::_on_unit_order_changed);

  // Bind all methods first
  ClassDB::bind_method(D_METHOD("set_base_attack_time", "bat"),
                       &AttackComponent::set_base_attack_time);
  ClassDB::bind_method(D_METHOD("get_base_attack_time"),
                       &AttackComponent::get_base_attack_time);

  ClassDB::bind_method(D_METHOD("set_attack_speed", "speed"),
                       &AttackComponent::set_attack_speed);
  ClassDB::bind_method(D_METHOD("get_attack_speed"),
                       &AttackComponent::get_attack_speed);

  ClassDB::bind_method(D_METHOD("set_attack_point", "seconds"),
                       &AttackComponent::set_attack_point);
  ClassDB::bind_method(D_METHOD("get_attack_point"),
                       &AttackComponent::get_attack_point);

  ClassDB::bind_method(D_METHOD("set_attack_range", "range"),
                       &AttackComponent::set_attack_range);
  ClassDB::bind_method(D_METHOD("get_attack_range"),
                       &AttackComponent::get_attack_range);

  ClassDB::bind_method(D_METHOD("set_attack_damage", "damage"),
                       &AttackComponent::set_attack_damage);
  ClassDB::bind_method(D_METHOD("get_attack_damage"),
                       &AttackComponent::get_attack_damage);

  ClassDB::bind_method(D_METHOD("set_delivery_type", "type"),
                       &AttackComponent::set_delivery_type);
  ClassDB::bind_method(D_METHOD("get_delivery_type"),
                       &AttackComponent::get_delivery_type);

  ClassDB::bind_method(D_METHOD("set_projectile_speed", "speed"),
                       &AttackComponent::set_projectile_speed);
  ClassDB::bind_method(D_METHOD("get_projectile_speed"),
                       &AttackComponent::get_projectile_speed);

  ClassDB::bind_method(D_METHOD("set_auto_attack_range", "range"),
                       &AttackComponent::set_auto_attack_range);
  ClassDB::bind_method(D_METHOD("get_auto_attack_range"),
                       &AttackComponent::get_auto_attack_range);

  ClassDB::bind_method(D_METHOD("set_attack_buffer_range", "buffer"),
                       &AttackComponent::set_attack_buffer_range);
  ClassDB::bind_method(D_METHOD("get_attack_buffer_range"),
                       &AttackComponent::get_attack_buffer_range);

  ClassDB::bind_method(D_METHOD("set_projectile_scene", "scene"),
                       &AttackComponent::set_projectile_scene);
  ClassDB::bind_method(D_METHOD("get_projectile_scene"),
                       &AttackComponent::get_projectile_scene);

  ClassDB::bind_method(D_METHOD("try_fire_at", "target", "delta"),
                       &AttackComponent::try_fire_at);
  ClassDB::bind_method(D_METHOD("get_attack_interval"),
                       &AttackComponent::get_attack_interval);

  // Add properties with group organization
  ADD_GROUP("Attack Settings", "");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "delivery_type",
                            godot::PROPERTY_HINT_ENUM, "Melee,Projectile"),
               "set_delivery_type", "get_delivery_type");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_attack_time"),
               "set_base_attack_time", "get_base_attack_time");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_speed"), "set_attack_speed",
               "get_attack_speed");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_point"), "set_attack_point",
               "get_attack_point");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_range"), "set_attack_range",
               "get_attack_range");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_damage"),
               "set_attack_damage", "get_attack_damage");

  ADD_GROUP("Projectile Settings", "");
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "projectile_scene",
                            godot::PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"),
               "set_projectile_scene", "get_projectile_scene");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "projectile_speed"),
               "set_projectile_speed", "get_projectile_speed");

  ADD_GROUP("Range & Hysteresis", "");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "auto_attack_range"),
               "set_auto_attack_range", "get_auto_attack_range");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_buffer_range"),
               "set_attack_buffer_range", "get_attack_buffer_range");

  ADD_SIGNAL(godot::MethodInfo("attack_started",
                               PropertyInfo(Variant::OBJECT, "target")));
  ADD_SIGNAL(godot::MethodInfo("attack_point_reached",
                               PropertyInfo(Variant::OBJECT, "target")));
  ADD_SIGNAL(godot::MethodInfo("attack_hit",
                               PropertyInfo(Variant::OBJECT, "target"),
                               PropertyInfo(Variant::FLOAT, "damage")));
}

void AttackComponent::_ready() {
  UnitComponent::_ready();

  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  Unit* owner = get_unit();
  if (owner == nullptr) {
    return;
  }

  // Connect to the Unit's order_changed signal
  owner->connect("order_changed",
                 godot::Callable(this, "_on_unit_order_changed"));
}

void AttackComponent::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Decrement cooldown timer
  if (time_until_next_attack > 0.0) {
    time_until_next_attack -= delta;
  }

  // Advance windup timer if in windup
  if (in_attack_windup) {
    attack_windup_timer += delta;

    // Check if we've reached the attack point
    if (attack_windup_timer >= attack_point) {
      if (current_attack_target != nullptr &&
          current_attack_target->is_inside_tree()) {
        HealthComponent* target_health = Object::cast_to<HealthComponent>(
            current_attack_target->get_component_by_class("HealthComponent"));

        if (target_health != nullptr && !target_health->is_dead()) {
          // Fire the attack
          if (delivery_type == AttackDelivery::MELEE) {
            _fire_melee(current_attack_target);
          } else if (delivery_type == AttackDelivery::PROJECTILE) {
            _fire_projectile(current_attack_target);
          }

          emit_signal("attack_point_reached", current_attack_target);
          time_until_next_attack = get_attack_interval();
        }
      }

      // Exit windup regardless
      in_attack_windup = false;
      current_attack_target = nullptr;
    }
  }

  // Handle active attack target
  if (active_attack_target != nullptr &&
      active_attack_target->is_inside_tree()) {
    HealthComponent* target_health = Object::cast_to<HealthComponent>(
        active_attack_target->get_component_by_class("HealthComponent"));

    if (target_health == nullptr || target_health->is_dead()) {
      // Target is dead, clear the order
      active_attack_target = nullptr;
      return;
    }

    // Check distance to target
    Unit* owner = get_unit();
    if (owner != nullptr) {
      float distance = owner->get_global_position().distance_to(
          active_attack_target->get_global_position());

      if (distance <= attack_range) {
        // In range: attempt to attack if cooldown is over
        if (!in_attack_windup && time_until_next_attack <= 0.0) {
          try_fire_at(active_attack_target, delta);
        }
      } else {
        // Out of range: issue chase order to move toward target
        owner->issue_chase_order(active_attack_target);
      }
    }
  }
}

void AttackComponent::set_base_attack_time(float bat) {
  base_attack_time = std::max(0.1f, bat);
}

float AttackComponent::get_base_attack_time() const {
  return base_attack_time;
}

void AttackComponent::set_attack_speed(float speed) {
  attack_speed = std::max(1.0f, speed);
}

float AttackComponent::get_attack_speed() const {
  return attack_speed;
}

void AttackComponent::set_attack_point(float seconds) {
  attack_point = std::max(0.0f, seconds);
}

float AttackComponent::get_attack_point() const {
  return attack_point;
}

void AttackComponent::set_attack_range(float range) {
  attack_range = std::max(0.1f, range);
}

float AttackComponent::get_attack_range() const {
  return attack_range;
}

void AttackComponent::set_attack_damage(float damage) {
  attack_damage = std::max(0.0f, damage);
}

float AttackComponent::get_attack_damage() const {
  return attack_damage;
}

void AttackComponent::set_delivery_type(int type) {
  if (type == 0) {
    delivery_type = AttackDelivery::MELEE;
  } else if (type == 1) {
    delivery_type = AttackDelivery::PROJECTILE;
  }
}

int AttackComponent::get_delivery_type() const {
  return static_cast<int>(delivery_type);
}

void AttackComponent::set_projectile_speed(float speed) {
  projectile_speed = std::max(0.1f, speed);
}

float AttackComponent::get_projectile_speed() const {
  return projectile_speed;
}

void AttackComponent::set_projectile_scene(const Ref<PackedScene>& scene) {
  projectile_scene = scene;
}

Ref<PackedScene> AttackComponent::get_projectile_scene() const {
  return projectile_scene;
}

void AttackComponent::set_auto_attack_range(float range) {
  auto_attack_range = range;
}

float AttackComponent::get_auto_attack_range() const {
  return auto_attack_range;
}

void AttackComponent::set_attack_buffer_range(float buffer) {
  attack_buffer_range = std::max(0.0f, buffer);
}

float AttackComponent::get_attack_buffer_range() const {
  return attack_buffer_range;
}

bool AttackComponent::try_fire_at(Unit* target, double delta) {
  if (target == nullptr || !target->is_inside_tree()) {
    return false;
  }

  // If we're not in windup and ready to attack
  if (!in_attack_windup && time_until_next_attack <= 0.0) {
    // Start windup
    in_attack_windup = true;
    attack_windup_timer = 0.0;
    current_attack_target = target;

    if (owner_unit != nullptr) {
      DBG_INFO("AttackComponent", "" + owner_unit->get_name() +
                              " started attacking " + target->get_name());
    }

    emit_signal("attack_started", target);
    return false;  // Attack hasn't landed yet
  }

  return false;
}

float AttackComponent::get_attack_interval() const {
  float attack_speed_factor = attack_speed / 100.0f;
  return base_attack_time / attack_speed_factor;
}

void AttackComponent::_fire_melee(Unit* target) {
  if (target == nullptr) {
    return;
  }

  HealthComponent* target_health = Object::cast_to<HealthComponent>(
      target->get_component_by_class("HealthComponent"));

  if (target_health == nullptr) {
    UtilityFunctions::push_error(
        "[AttackComponent] Target missing HealthComponent");
    return;
  }

  if (owner_unit != nullptr) {
    DBG_INFO("AttackComponent", "" + owner_unit->get_name() +
                            " hit " + target->get_name() + " for " +
                            String::num(attack_damage) + " damage (MELEE)");
  }

  target_health->apply_damage(attack_damage, owner_unit);
  emit_signal("attack_hit", target, attack_damage);
}

void AttackComponent::_fire_projectile(Unit* target) {
  if (target == nullptr) {
    return;
  }

  if (projectile_scene.is_null()) {
    UtilityFunctions::push_error(
        "[AttackComponent] Projectile attack configured but projectile_scene "
        "is not set");
    return;
  }

  // Spawn projectile
  Node* projectile_node = projectile_scene->instantiate();
  auto projectile = Object::cast_to<Projectile>(projectile_node);

  if (projectile == nullptr) {
    UtilityFunctions::push_error(
        "[AttackComponent] Projectile scene root must be a Projectile node");
    projectile_node->queue_free();
    return;
  }

  // Add to parent first
  Node* parent = get_parent();
  if (parent != nullptr) {
    parent->add_child(projectile);
  }

  if (owner_unit != nullptr) {
    DBG_INFO("AttackComponent", "" + owner_unit->get_name() +
                            " fired projectile at " + target->get_name() +
                            " (damage: " + String::num(attack_damage) + ")");
  }

  // Configure projectile with pre-calculated damage
  projectile->setup(owner_unit, target, attack_damage, projectile_speed);

  emit_signal("attack_hit", target, attack_damage);
}

void AttackComponent::_on_unit_order_changed(int previous_order,
                                             int new_order,
                                             Object* target) {
  // OrderType::ATTACK = 2
  if (new_order == 2) {
    Unit* target_unit = Object::cast_to<Unit>(target);
    if (target_unit != nullptr) {
      // Set the active attack target
      active_attack_target = target_unit;
      // Try to fire at the target
      // The _physics_process will handle cooldown timing and repeat attacks
      try_fire_at(target_unit, 0.0);
    }
  } else if (new_order == 3) {
    // OrderType::CHASE = 3
    Unit* target_unit = Object::cast_to<Unit>(target);
    if (target_unit != nullptr) {
      // Keep the attack target active for when we get back in range
      if (active_attack_target == nullptr) {
        active_attack_target = target_unit;
      }
    }
  } else {
    // Clear attack order when order changes to something else
    active_attack_target = nullptr;
  }
}
