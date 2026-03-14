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

  ClassDB::bind_method(D_METHOD("_on_cooldown_started", "slot", "duration"),
                       &CooldownDisplayComponent::_on_cooldown_started);
  ClassDB::bind_method(D_METHOD("_on_cooldown_tick", "slot", "remaining_time"),
                       &CooldownDisplayComponent::_on_cooldown_tick);
}

void CooldownDisplayComponent::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  show();

  // Find MatchManager in scene
  Node* current = this;
  Node* scene_root = nullptr;
  while (current && current->get_parent()) {
    scene_root = current;
    current = current->get_parent();
  }

  if (scene_root) {
    for (int i = 0; i < scene_root->get_child_count(); i++) {
      match_manager = Object::cast_to<MatchManager>(scene_root->get_child(i));
      if (match_manager) break;
    }
  }

  if (!match_manager) {
    DBG_WARN("CooldownDisplay", "MatchManager not found");
    return;
  }

  main_unit = match_manager->get_main_unit();
  if (!main_unit) {
    DBG_WARN("CooldownDisplay", "No main_unit in MatchManager");
    return;
  }

  // Find child labels via paths
  for (const Variant& label_path_var : ability_slot_label_paths) {
    Label* label = Object::cast_to<Label>(
        get_node_or_null(godot::NodePath(label_path_var)));
    if (label) {
      cooldown_labels.push_back(label);
    }
  }

  // Initialize cooldown timers for 4 ability slots
  cooldown_timers.resize(4);
  for (int i = 0; i < 4; i++) {
    cooldown_timers[i].slot = i;
  }

  // Connect to ability cooldown signals from main_unit
  main_unit->connect(
      ability_cooldown_started,
      godot::Callable(this, godot::StringName("_on_cooldown_started")));
  main_unit->connect(
      ability_cooldown_tick,
      godot::Callable(this, godot::StringName("_on_cooldown_tick")));

  DBG_INFO("CooldownDisplay",
           "Initialized with " + String::num(cooldown_labels.size()) +
               " labels");
}

void CooldownDisplayComponent::_process(double delta) {}

void CooldownDisplayComponent::_on_cooldown_started(int slot, float duration) {
  if (slot < 0) return;

  if (slot >= static_cast<int>(cooldown_timers.size())) {
    cooldown_timers.resize(slot + 1);
    for (size_t i = 0; i < cooldown_timers.size(); i++) {
      cooldown_timers[i].slot = i;
    }
  }

  CooldownTimer& timer = cooldown_timers[slot];
  timer.slot = slot;
  timer.duration = duration;
  timer.elapsed = 0.0f;
  timer.active = true;
}

void CooldownDisplayComponent::_on_cooldown_tick(int slot,
                                                 float remaining_time) {
  if (slot < 0 || slot >= static_cast<int>(cooldown_labels.size())) return;
  _update_label(slot, remaining_time);
}

String CooldownDisplayComponent::_format_cooldown(float remaining) const {
  if (remaining <= 0.0f) return "";
  if (remaining >= 1.0f) return String::num((int)remaining) + "s";
  return String::num(remaining, 1) + "s";
}

void CooldownDisplayComponent::_update_label(int slot_index, float remaining) {
  if (slot_index < 0 || slot_index >= static_cast<int>(cooldown_labels.size()))
    return;

  Label* label = cooldown_labels[slot_index];
  if (!label) return;

  String new_text = _format_cooldown(remaining);
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
