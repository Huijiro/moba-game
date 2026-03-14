#include "projectile.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Object;
using godot::PropertyInfo;
using godot::Variant;

Projectile::Projectile() = default;

Projectile::~Projectile() = default;

void Projectile::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_body_entered", "body"),
                       &Projectile::_on_body_entered);
}

void Projectile::_ready() {
  set_physics_process(false);
  set_monitoring(false);
}

void Projectile::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (target == nullptr || !target->is_inside_tree()) {
    queue_free();
    return;
  }

  Vector3 current_pos = get_global_position();
  Vector3 target_pos = target->get_global_position();

  // Recompute direction each frame (target is moving)
  Vector3 to_target = target_pos - current_pos;
  float distance_to_target = to_target.length();

  // Move towards target
  if (distance_to_target > 0.001f) {
    direction = to_target / distance_to_target;
    Vector3 velocity = direction * speed;
    set_global_position(current_pos + velocity * static_cast<float>(delta));
  }
}

void Projectile::_on_body_entered(godot::Node3D* body) {
  Unit* unit = Object::cast_to<Unit>(body);
  if (unit == nullptr || unit != target) {
    return;  // Only hit the intended target
  }

  if (attacker != nullptr) {
    DBG_INFO("Projectile", "" + attacker->get_name() + "'s projectile hit " +
                               target->get_name() + " for " +
                               godot::String::num(damage) + " damage");
  }
  target->relay("take_damage", damage, attacker);
  queue_free();
}

void Projectile::setup(Unit* attacker_unit,
                       Unit* target_unit,
                       float damage_amount,
                       float travel_speed) {
  attacker = attacker_unit;
  target = target_unit;
  damage = damage_amount;
  speed = travel_speed;

  if (target != nullptr && attacker != nullptr) {
    Vector3 to_target =
        target->get_global_position() - attacker->get_global_position();
    float distance = to_target.length();
    if (distance > 0.001f) {
      direction = to_target / distance;
    }
  }

  set_physics_process(true);
  set_monitoring(true);
  connect("body_entered", godot::Callable(this, "_on_body_entered"));
}
