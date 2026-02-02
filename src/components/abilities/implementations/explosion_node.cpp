#include "explosion_node.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../core/unit.hpp"
#include "../../health/health_component.hpp"

using godot::Array;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Node;
using godot::Object;
using godot::SceneTree;
using godot::String;
using godot::UtilityFunctions;
using godot::Vector3;

ExplosionNode::ExplosionNode() {
  // Set sensible defaults for explosion
  set_ability_name("Explosion");
  set_description(
      "Instant AoE ability dealing damage to all units in radius around "
      "caster");
  set_cast_type(static_cast<int>(CastType::INSTANT));
  set_targeting_type(static_cast<int>(TargetingType::AREA));
  set_base_damage(60.0f);
  set_aoe_radius(8.0f);
  set_cooldown(10.0f);
}

ExplosionNode::~ExplosionNode() = default;

void ExplosionNode::_bind_methods() {
  // No custom properties for ExplosionNode, inherits all from AbilityNode
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "position"),
                       &ExplosionNode::execute);
  ClassDB::bind_method(D_METHOD("can_execute_on_target", "caster", "target"),
                       &ExplosionNode::can_execute_on_target);
  ClassDB::bind_method(D_METHOD("calculate_damage", "caster", "target"),
                       &ExplosionNode::calculate_damage);
  ClassDB::bind_method(D_METHOD("query_units_in_area", "center"),
                       &ExplosionNode::query_units_in_area);
}

void ExplosionNode::execute(Unit* caster, Unit* target, Vector3 position) {
  if (caster == nullptr) {
    UtilityFunctions::print("[Explosion] No caster provided");
    return;
  }

  // For self-cast abilities, the impact point is the caster's position
  Vector3 impact_point = caster->get_global_position();

  // Find all units in the area
  Array units_in_area = query_units_in_area(impact_point);

  if (units_in_area.is_empty()) {
    UtilityFunctions::print("[Explosion] No units found in explosion area");
    return;
  }

  // Apply damage to all units in area
  int hit_count = 0;
  for (int i = 0; i < units_in_area.size(); i++) {
    Unit* affected_unit = Object::cast_to<Unit>(units_in_area[i]);
    if (affected_unit == nullptr || !affected_unit->is_inside_tree()) {
      continue;
    }

    // Don't damage the caster
    if (affected_unit == caster) {
      continue;
    }

    // Get health component
    HealthComponent* affected_health = Object::cast_to<HealthComponent>(
        affected_unit->get_component_by_class("HealthComponent"));

    if (affected_health == nullptr) {
      continue;
    }

    // Apply damage
    float damage = calculate_damage(caster, affected_unit);
    affected_health->apply_damage(damage, caster);
    hit_count++;

    UtilityFunctions::print("[Explosion] Hit " + affected_unit->get_name() +
                            " for " + String::num(damage) + " damage");
  }

  UtilityFunctions::print("[Explosion] " + caster->get_name() +
                          " detonated, hit " + String::num(hit_count) +
                          " units");
}

bool ExplosionNode::can_execute_on_target(Unit* caster, Unit* target) const {
  if (caster == nullptr) {
    return false;
  }

  // Self-cast ability - only needs a valid caster
  return true;
}

float ExplosionNode::calculate_damage(Unit* caster, Unit* target) const {
  // Simple damage: just return base damage
  // Subclasses can override to add scaling (e.g., based on caster stats)
  return get_base_damage();
}

Array ExplosionNode::query_units_in_area(const Vector3& center) const {
  Array result;

  // Get the scene tree using Godot's built-in method
  // Cast away const since get_tree() is const-correct in Godot
  ExplosionNode* mutable_this = const_cast<ExplosionNode*>(this);
  SceneTree* scene_tree = mutable_this->get_tree();

  if (scene_tree == nullptr || !mutable_this->is_inside_tree()) {
    UtilityFunctions::print("[Explosion] Cannot access scene tree");
    return result;
  }

  // Find the root by traversing up
  Node* current = mutable_this;
  Node* root = mutable_this;
  while (current != nullptr) {
    root = current;
    current = current->get_parent();
  }

  // Calculate radius squared for distance checks
  float radius = get_aoe_radius();
  float radius_sq = radius * radius;

  // Search for all units in the area
  _search_units_in_tree(root, center, radius_sq, result);

  return result;
}

void ExplosionNode::_search_units_in_tree(Node* node,
                                          const Vector3& center,
                                          float radius_sq,
                                          Array& result) const {
  if (node == nullptr) {
    return;
  }

  // Check if current node is a Unit
  Unit* unit = Object::cast_to<Unit>(node);
  if (unit != nullptr && unit->is_inside_tree()) {
    Vector3 unit_pos = unit->get_global_position();
    float distance_sq = center.distance_squared_to(unit_pos);

    if (distance_sq <= radius_sq) {
      result.append(unit);
    }
  }

  // Recursively search children (limit depth to avoid stack overflow)
  for (int i = 0; i < node->get_child_count(); i++) {
    Node* child = node->get_child(i);
    if (child != nullptr) {
      _search_units_in_tree(child, center, radius_sq, result);
    }
  }
}
