#include "targeting_preview.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "../components/abilities/ability_component.hpp"
#include "../components/abilities/ability_components/aoe_damage_component.hpp"
#include "../components/abilities/ability_node.hpp"
#include "../components/abilities/ability_subcomponent.hpp"
#include "../core/unit.hpp"
#include "../debug/debug_macros.hpp"
#include "ability_targeting_handler.hpp"
#include "cursor_world_query.hpp"

using godot::ClassDB;
using godot::Engine;
using godot::Node3D;
using godot::Object;
using godot::String;
using godot::Vector3;

TargetingPreview::TargetingPreview() = default;
TargetingPreview::~TargetingPreview() = default;

void TargetingPreview::_bind_methods() {
  ClassDB::bind_method(godot::D_METHOD("set_controlled_unit", "unit"),
                       &TargetingPreview::set_controlled_unit);
  ClassDB::bind_method(godot::D_METHOD("get_controlled_unit"),
                       &TargetingPreview::get_controlled_unit);
}

void TargetingPreview::set_controlled_unit(Unit* unit) {
  controlled_unit = unit;
}
Unit* TargetingPreview::get_controlled_unit() const {
  return controlled_unit;
}

void TargetingPreview::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) return;

  godot::Node* parent = get_parent();
  if (parent == nullptr) return;

  for (int i = 0; i < parent->get_child_count(); i++) {
    godot::Node* child = parent->get_child(i);
    if (cursor_query == nullptr)
      cursor_query = Object::cast_to<CursorWorldQuery>(child);
    if (targeting_handler == nullptr)
      targeting_handler = Object::cast_to<AbilityTargetingHandler>(child);
  }
}

void TargetingPreview::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) return;
  if (targeting_handler == nullptr || controlled_unit == nullptr) {
    return;
  }

  bool handler_active = targeting_handler->is_active();
  int handler_slot = targeting_handler->get_active_slot();

  if (handler_active && !previews_active) {
    // Just entered targeting mode
    _activate_previews(handler_slot);
  } else if (!handler_active && previews_active) {
    // Just left targeting mode
    _deactivate_previews();
  } else if (handler_active && handler_slot != current_slot) {
    // Switched ability slot while targeting
    _deactivate_previews();
    _activate_previews(handler_slot);
  }

  if (previews_active) {
    _update_previews();
  }
}

void TargetingPreview::_activate_previews(int slot) {
  AbilityNode* ability = _find_ability(slot);
  if (ability == nullptr) {
    DBG_WARN("TargetingPreview",
             "No ability found for slot " + String::num(slot));
    return;
  }
  DBG_INFO("TargetingPreview",
           "Activating previews for " + ability->get_ability_name() +
               " (slot " + String::num(slot) + "), children: " +
               String::num(ability->get_child_count()));

  // Walk children of the ability node, collecting preview providers
  for (int i = 0; i < ability->get_child_count(); i++) {
    godot::Node* child = ability->get_child(i);

    // Check AbilitySubcomponent
    auto* subcomp = Object::cast_to<AbilitySubcomponent>(child);
    if (subcomp != nullptr && subcomp->has_preview()) {
      Node3D* preview = subcomp->create_preview();
      if (preview != nullptr) {
        get_tree()->get_root()->add_child(preview);
        PreviewEntry entry;
        entry.component = subcomp;
        entry.preview_node = preview;
        active_previews.push_back(entry);
      }
      continue;
    }

    // Check AoEDamageComponent (extends Area3D, not AbilitySubcomponent)
    auto* aoe = Object::cast_to<AoEDamageComponent>(child);
    if (aoe != nullptr && aoe->has_preview()) {
      Node3D* preview = aoe->create_preview();
      if (preview != nullptr) {
        get_tree()->get_root()->add_child(preview);
        PreviewEntry entry;
        entry.aoe_component = aoe;
        entry.preview_node = preview;
        active_previews.push_back(entry);
      }
    }
  }

  previews_active = true;
  current_slot = slot;

  DBG_DEBUG("TargetingPreview",
            "Activated " + String::num(active_previews.size()) +
                " previews for slot " + String::num(slot));
}

void TargetingPreview::_update_previews() {
  if (cursor_query == nullptr || controlled_unit == nullptr) return;

  Vector3 caster_pos = controlled_unit->get_global_position();
  Vector3 ground_pos = cursor_query->has_ground_hit()
                           ? cursor_query->get_ground_position()
                           : caster_pos;

  for (int i = 0; i < active_previews.size(); i++) {
    const PreviewEntry& entry = active_previews[i];
    if (entry.component != nullptr) {
      entry.component->update_preview(entry.preview_node, caster_pos,
                                      ground_pos);
    } else if (entry.aoe_component != nullptr) {
      entry.aoe_component->update_preview(entry.preview_node, caster_pos,
                                          ground_pos);
    }
  }
}

void TargetingPreview::_deactivate_previews() {
  for (int i = 0; i < active_previews.size(); i++) {
    const PreviewEntry& entry = active_previews[i];
    if (entry.preview_node != nullptr) {
      if (entry.component != nullptr) {
        entry.component->cleanup_preview(entry.preview_node);
      } else if (entry.aoe_component != nullptr) {
        entry.aoe_component->cleanup_preview(entry.preview_node);
      } else {
        entry.preview_node->queue_free();
      }
    }
  }
  active_previews.clear();
  previews_active = false;
  current_slot = -1;
}

AbilityNode* TargetingPreview::_find_ability(int slot) const {
  if (controlled_unit == nullptr) return nullptr;

  // Find AbilityComponent on unit
  for (int i = 0; i < controlled_unit->get_child_count(); i++) {
    auto* comp =
        Object::cast_to<AbilityComponent>(controlled_unit->get_child(i));
    if (comp != nullptr) {
      return comp->get_ability(slot);
    }
  }
  return nullptr;
}
