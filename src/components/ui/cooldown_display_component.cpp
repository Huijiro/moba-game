#include "cooldown_display_component.hpp"

#include <algorithm>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../common/unit_signals.hpp"
#include "../../core/match_manager.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::Variant;

CooldownDisplayComponent::CooldownDisplayComponent() = default;

CooldownDisplayComponent::~CooldownDisplayComponent() = default;

void CooldownDisplayComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_ability_slot_label_paths", "paths"),
                       &CooldownDisplayComponent::set_ability_slot_label_paths);
  ClassDB::bind_method(D_METHOD("get_ability_slot_label_paths"),
                       &CooldownDisplayComponent::get_ability_slot_label_paths);
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "ability_slot_label_paths",
                            godot::PROPERTY_HINT_ARRAY_TYPE, "NodePath"),
               "set_ability_slot_label_paths", "get_ability_slot_label_paths");

  // Signal handlers
  ClassDB::bind_method(D_METHOD("_on_cooldown_started", "slot", "duration"),
                       &CooldownDisplayComponent::_on_cooldown_started);
  ClassDB::bind_method(D_METHOD("_on_cooldown_tick", "slot", "remaining_time"),
                       &CooldownDisplayComponent::_on_cooldown_tick);
}

void CooldownDisplayComponent::_ready() {
  DBG_INFO("CooldownDisplay", "CooldownDisplayComponent _ready called");
  DBG_INFO("CooldownDisplay",
           "  Visible: " + String(is_visible() ? "yes" : "no") +
               ", Class: " + String(get_class()));

  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Make sure this node is visible
  show();
  DBG_INFO("CooldownDisplay",
           "  After show - Visible: " + String(is_visible() ? "yes" : "no"));

  // Debug parent hierarchy
  Node* parent = get_parent();
  if (parent) {
    DBG_INFO("CooldownDisplay", "  Parent: " + parent->get_name() +
                                    " (class: " + String(parent->get_class()) +
                                    ")");
    if (Object::cast_to<godot::CanvasLayer>(parent)) {
      godot::CanvasLayer* canvas = Object::cast_to<godot::CanvasLayer>(parent);
      DBG_INFO("CooldownDisplay",
               "    CanvasLayer visible: " +
                   String(canvas->is_visible() ? "yes" : "no"));
    }
  }

  // 1. Find MatchManager in scene
  // Search up to find the scene root, then look for MatchManager sibling
  Node* current = this;
  Node* scene_root = nullptr;

  while (current && current->get_parent()) {
    scene_root = current;
    current = current->get_parent();
  }

  // Try to find MatchManager as child of scene root
  if (scene_root) {
    for (int i = 0; i < scene_root->get_child_count(); i++) {
      match_manager = Object::cast_to<MatchManager>(scene_root->get_child(i));
      if (match_manager) {
        break;
      }
    }
  }

  if (!match_manager) {
    DBG_WARN("CooldownDisplay", "MatchManager not found in scene");
    return;
  }

  // 2. Get main_unit from MatchManager
  main_unit = match_manager->get_main_unit();
  if (!main_unit) {
    DBG_WARN("CooldownDisplay", "No main_unit set in MatchManager");
    return;
  }

  // 3. Find child labels via paths
  for (const Variant& label_path_var : ability_slot_label_paths) {
    Label* label = Object::cast_to<Label>(
        get_node_or_null(godot::NodePath(label_path_var)));
    if (label) {
      cooldown_labels.push_back(label);
    } else {
      DBG_WARN("CooldownDisplay",
               "Label not found at path: " + String(label_path_var));
    }
  }

  // 4. Initialize cooldown timers - we'll know the count once signals start
  // For now, just initialize an empty vector - it will grow as needed
  cooldown_timers.resize(4);  // Assume 4 ability slots (Q, W, E, R)
  for (int i = 0; i < 4; i++) {
    cooldown_timers[i].slot = i;
  }

  // 5. Connect to ability cooldown signals from main_unit
  main_unit->connect(
      ability_cooldown_started,
      godot::Callable(this, godot::StringName("_on_cooldown_started")));
  main_unit->connect(
      ability_cooldown_tick,
      godot::Callable(this, godot::StringName("_on_cooldown_tick")));

  DBG_INFO("CooldownDisplay", "Initialized with " +
                                  String::num(cooldown_labels.size()) +
                                  " labels for ability cooldown tracking");
}

void CooldownDisplayComponent::_process(double delta) {
  // Process is no longer used - signals handle all updates
}

void CooldownDisplayComponent::_on_cooldown_started(int slot, float duration) {
  if (slot < 0 || slot >= static_cast<int>(cooldown_timers.size())) {
    // Resize if needed
    if (slot >= 0) {
      cooldown_timers.resize(slot + 1);
      for (size_t i = 0; i < cooldown_timers.size(); i++) {
        cooldown_timers[i].slot = i;
      }
    }
  }

  if (slot < 0 || slot >= static_cast<int>(cooldown_timers.size())) {
    DBG_WARN("CooldownDisplay", "Invalid slot: " + String::num(slot));
    return;
  }

  CooldownTimer& timer = cooldown_timers[slot];
  timer.slot = slot;
  timer.duration = duration;
  timer.elapsed = 0.0f;
  timer.active = true;

  DBG_DEBUG("CooldownDisplay", "Started cooldown: slot=" + String::num(slot) +
                                   " duration=" + String::num(duration, 2));
}

void CooldownDisplayComponent::_on_cooldown_tick(int slot,
                                                 float remaining_time) {
  if (slot < 0 || slot >= static_cast<int>(cooldown_labels.size())) {
    return;
  }

  _update_label(slot, remaining_time);
}

String CooldownDisplayComponent::_format_cooldown(float remaining) const {
  if (remaining <= 0.0f) {
    return "";
  }

  if (remaining >= 1.0f) {
    // Format as "3s", "2s", "1s"
    return String::num((int)remaining) + "s";
  }

  // Format as "0.9s", "0.5s", "0.1s"
  String formatted = String::num(remaining, 1) + "s";
  return formatted;
}

void CooldownDisplayComponent::_update_label(int slot_index, float remaining) {
  if (slot_index < 0 ||
      slot_index >= static_cast<int>(cooldown_labels.size())) {
    return;
  }

  Label* label = cooldown_labels[slot_index];
  if (!label) {
    return;
  }

  String new_text = _format_cooldown(remaining);

  // Only update if text changed
  if (label->get_text() != new_text) {
    label->set_text(new_text);
  }
}

void CooldownDisplayComponent::set_ability_slot_label_paths(
    const godot::Array& paths) {
  ability_slot_label_paths = paths;
}

godot::Array CooldownDisplayComponent::get_ability_slot_label_paths() const {
  return ability_slot_label_paths;
}
