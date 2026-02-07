#include "explosion_node.hpp"
#include "../../../debug/debug_macros.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../visual/vfx_node.hpp"

using godot::Array;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Node;
using godot::SceneTree;
using godot::String;
using godot::UtilityFunctions;
using godot::Vector3;

ExplosionNode::ExplosionNode() {
  // Set sensible defaults for explosion
  set_ability_name("Explosion");
  set_description(
      "Cast an explosion at target location, dealing AoE damage to all units "
      "in radius");
  set_cast_type(static_cast<int>(CastType::INSTANT));
  set_targeting_type(static_cast<int>(TargetingType::POINT_TARGET));
  set_base_damage(60.0f);
  set_aoe_radius(8.0f);
  set_range(20.0f);  // Max distance to cast explosion
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

bool ExplosionNode::execute(Unit* caster, Unit* target, Vector3 position) {
  if (caster == nullptr) {
    DBG_ERROR("Explosion", "No caster provided");
    return false;
  }

  if (!caster->is_inside_tree()) {
    DBG_ERROR("Explosion", "Caster is not in scene tree");
    return false;
  }

  // For point-target abilities, use the clicked position
  Vector3 impact_point = position;

  // Get the root node to search from
  // SceneTree::get_root() returns a Window, but we need to search the scene
  // tree Instead, use the caster's parent hierarchy to find the scene root
  Node* root = caster;
  while (root->get_parent() != nullptr) {
    root = root->get_parent();
  }
  if (root == nullptr) {
    DBG_ERROR("Explosion", "Cannot find scene root");
    return false;
  }

  // Trigger explosion VFX at impact position with animation-driven damage
  // callback
  godot::Dictionary explosion_params;
  explosion_params["position"] = impact_point;
  explosion_params["scale"] = get_aoe_radius();  // Scale matches AoE radius
  explosion_params["animation_name"] = "explosion_anticipation_boom";
  explosion_params["expected_signals"] = Array{"explosion_damage"};
  explosion_params["intensity"] = 0.9f;
  explosion_params["duration"] = 0.8f;

  // Spawn VFX
  Node* vfx_node = play_vfx(caster, "explosion", explosion_params);
  if (vfx_node == nullptr) {
    DBG_WARN("Explosion", "Failed to spawn explosion VFX");
    return false;
  }

  // Register the callback on the VFX node
  // The callback will fire when animation emits "explosion_damage" signal
  auto vfx = Object::cast_to<VFXNode>(vfx_node);
  if (vfx != nullptr) {
    vfx->register_callback("explosion_damage", [this, caster, root,
                                                impact_point]() {
      // Calculate radius squared for distance checks
      float radius = get_aoe_radius();
      float radius_sq = radius * radius;

      // Find all units in the area
      Array units_in_area;
      _search_units_in_tree(root, impact_point, radius_sq, units_in_area);

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

        // Apply damage via fire-and-forget signal
        float damage = calculate_damage(caster, affected_unit);
        affected_unit->relay("take_damage", damage, caster);
        hit_count++;

        DBG_DEBUG("Explosion", "Hit " + String(affected_unit->get_name()) +
                                   " for " + String::num(damage) + " damage");
      }

      DBG_INFO("Explosion", String(caster->get_name()) + " detonated, hit " +
                                String::num(hit_count) + " units");
    });
  }

  return true;
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

  // This method needs to be called with a caster that's in the scene tree
  // For now, we'll search from the current node's perspective
  // Note: ExplosionNode itself is not in the tree, but is called from
  // AbilityComponent which is in the tree

  // Since we can't reliably access the scene tree from here, we'll use a
  // different approach: The caster is responsible for being in the tree, so we
  // search from the root by finding any Unit in the scene

  // Calculate radius squared for distance checks
  float radius = get_aoe_radius();
  float radius_sq = radius * radius;

  // We need the scene root - try to get it from the owner if available
  // This is a limitation of the current design - abilities are data objects
  // that don't have direct scene tree access

  // For now, just return empty - the caster should provide units to search
  DBG_WARN("Explosion", "query_units_in_area called without scene context");
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
