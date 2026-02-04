#include "aoe_damage_effect.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>

#include "../../../common/unit_signals.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../../../debug/visual_debugger.hpp"
#include "../ability_definition.hpp"

using godot::ClassDB;
using godot::Color;
using godot::D_METHOD;
using godot::Engine;
using godot::Node;
using godot::Object;
using godot::UtilityFunctions;

AoEDamageEffect::AoEDamageEffect() = default;

AoEDamageEffect::~AoEDamageEffect() = default;

void AoEDamageEffect::_bind_methods() {
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "ability"),
                       &AoEDamageEffect::execute);
}

void AoEDamageEffect::execute(Unit* caster,
                              Object* target,
                              const AbilityDefinition* ability) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (caster == nullptr || ability == nullptr) {
    DBG_INFO("AoEDamageEffect", "Invalid caster or ability");
    return;
  }

  // Determine center of AoE
  Vector3 aoe_center = caster->get_global_position();  // Default to caster

  // If target is a Unit, use its position
  if (target != nullptr) {
    Unit* target_unit = Object::cast_to<Unit>(target);
    if (target_unit != nullptr && target_unit->is_inside_tree()) {
      aoe_center = target_unit->get_global_position();
      DBG_INFO("AoEDamageEffect", "Centering AoE on target unit");
    }
  } else {
    // For point-target abilities, AoE should damage nearby units around center
    // If target is null, use caster position
    // (Position-based abilities will pass Unit objects or be handled specially)
    DBG_INFO("AoEDamageEffect", "Centering AoE on caster position");
  }

  float radius = ability->get_aoe_radius();
  float damage = ability->get_base_damage();

  _apply_damage_in_radius(caster, aoe_center, radius, damage);

  // Debug visualization: Draw AoE circle
  VisualDebugger* debugger = VisualDebugger::get_singleton();
  if (debugger != nullptr && debugger->is_debug_enabled()) {
    // Draw yellow circle for AoE area
    debugger->draw_circle_xz(aoe_center, radius, Color(1, 1, 0, 0.5f));
  }

  DBG_INFO("AoEDamageEffect", "Applied " + godot::String::num(damage) +
                                  " damage in radius " +
                                  godot::String::num(radius));
}

void AoEDamageEffect::execute_at_point(Unit* caster,
                                       const Vector3& point,
                                       const AbilityDefinition* ability) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (caster == nullptr || ability == nullptr) {
    DBG_INFO("AoEDamageEffect", "Invalid caster or ability");
    return;
  }

  float radius = ability->get_aoe_radius();
  float damage = ability->get_base_damage();

  // Apply damage centered at the clicked point (not caster position)
  _apply_damage_in_radius(caster, point, radius, damage);

  // Debug visualization: Draw AoE circle
  VisualDebugger* debugger = VisualDebugger::get_singleton();
  if (debugger != nullptr && debugger->is_debug_enabled()) {
    // Draw yellow circle for AoE area
    debugger->draw_circle_xz(point, radius, Color(1, 1, 0, 0.5f));
  }

  DBG_INFO("AoEDamageEffect",
           "Applied " + godot::String::num(damage) + " damage at point (" +
               godot::String::num(point.x) + ", " +
               godot::String::num(point.z) + ") with radius " +
               godot::String::num(radius));
}

void AoEDamageEffect::_apply_damage_in_radius(Unit* caster,
                                              const Vector3& center,
                                              float radius,
                                              float damage) {
  if (caster == nullptr || !caster->is_inside_tree()) {
    return;
  }

  // Start from caster's parent node
  Node* start = caster->get_parent();
  if (start == nullptr) {
    return;
  }

  std::vector<Unit*> affected_units;
  std::vector<Node*> to_process;
  to_process.push_back(start);

  // Walk the node tree to find all Units within radius
  while (!to_process.empty()) {
    Node* current = to_process.back();
    to_process.pop_back();

    if (current == nullptr) {
      continue;
    }

    // Check if this node is a Unit (but not the caster)
    Unit* unit = Object::cast_to<Unit>(current);
    if (unit != nullptr && unit != caster && unit->is_inside_tree()) {
      float distance = center.distance_to(unit->get_global_position());
      if (distance <= radius) {
        affected_units.push_back(unit);
      }
    }

    // Add children to process queue
    for (int i = 0; i < current->get_child_count(); i++) {
      to_process.push_back(current->get_child(i));
    }
  }

  // Apply damage to all affected units (fire-and-forget via signals)
  int hit_count = 0;
  for (Unit* unit : affected_units) {
    caster->relay(take_damage, damage, unit);
    hit_count++;
    DBG_INFO("AoEDamageEffect", "Hit " + unit->get_name() + " for " +
                                    godot::String::num(damage) + " damage");
  }

  DBG_INFO("AoEDamageEffect", "Total hits: " + godot::String::num(hit_count));
}
