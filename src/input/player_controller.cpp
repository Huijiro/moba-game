#include "player_controller.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/string_name.hpp>

#include "../common/unit_signals.hpp"
#include "../core/game_settings.hpp"
#include "../core/unit.hpp"
#include "../debug/debug_macros.hpp"
#include "ability_targeting_handler.hpp"
#include "click_marker_handler.hpp"
#include "cursor_world_query.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::InputEventMouseButton;
using godot::Object;
using godot::PropertyInfo;
using godot::String;
using godot::StringName;
using godot::Variant;
using godot::Vector3;

PlayerController::PlayerController() = default;
PlayerController::~PlayerController() = default;

void PlayerController::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_controlled_unit", "unit"),
                       &PlayerController::set_controlled_unit);
  ClassDB::bind_method(D_METHOD("get_controlled_unit"),
                       &PlayerController::get_controlled_unit);

  ClassDB::bind_method(D_METHOD("set_move_action", "action"),
                       &PlayerController::set_move_action);
  ClassDB::bind_method(D_METHOD("get_move_action"),
                       &PlayerController::get_move_action);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "move_action"), "set_move_action",
               "get_move_action");

  ClassDB::bind_method(D_METHOD("set_cast_action", "action"),
                       &PlayerController::set_cast_action);
  ClassDB::bind_method(D_METHOD("get_cast_action"),
                       &PlayerController::get_cast_action);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "cast_action"), "set_cast_action",
               "get_cast_action");
}

void PlayerController::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) return;

  // Find sibling nodes
  godot::Node* parent = get_parent();
  if (parent != nullptr) {
    for (int i = 0; i < parent->get_child_count(); i++) {
      godot::Node* child = parent->get_child(i);
      if (cursor_query == nullptr)
        cursor_query = Object::cast_to<CursorWorldQuery>(child);
      if (targeting_handler == nullptr)
        targeting_handler = Object::cast_to<AbilityTargetingHandler>(child);
      if (click_marker == nullptr)
        click_marker = Object::cast_to<ClickMarkerHandler>(child);
    }
  }

  // Auto-find Unit if not set
  if (controlled_unit == nullptr && parent != nullptr) {
    // Search scene root for a Unit with matching conditions
    // For now, check if parent is the scene root and look for Unit children
  }

  _init_default_keybinds();
}

void PlayerController::_input(const godot::Ref<godot::InputEvent>& event) {
  if (Engine::get_singleton()->is_editor_hint()) return;
  if (controlled_unit == nullptr || cursor_query == nullptr) return;

  // Stop command
  if (event->is_action_pressed(StringName("game_stop"))) {
    _handle_stop();
    get_viewport()->set_input_as_handled();
    return;
  }

  // Ability keys (game_ability_1 through game_ability_6)
  for (int i = 1; i <= 6; i++) {
    String action_name = String("game_ability_") + String::num(i, 0);
    if (event->is_action_pressed(StringName(action_name))) {
      _handle_ability_key(action_name);
      get_viewport()->set_input_as_handled();
      return;
    }
  }

  // Mouse clicks
  auto* mouse_event = Object::cast_to<InputEventMouseButton>(event.ptr());
  if (mouse_event == nullptr || !mouse_event->is_pressed()) return;

  bool is_cast = event->is_action_pressed(StringName(cast_action));
  bool is_move = event->is_action_pressed(StringName(move_action));

  // If in targeting mode and left-click, delegate to targeting handler
  if (targeting_handler != nullptr && targeting_handler->is_active() &&
      is_cast) {
    targeting_handler->handle_click(cursor_query);
    get_viewport()->set_input_as_handled();
    return;
  }

  // Right-click: move or attack
  if (is_move && !is_cast) {
    // Cancel targeting on right-click
    if (targeting_handler != nullptr) {
      targeting_handler->cancel();
    }
    _handle_move_click();
    get_viewport()->set_input_as_handled();
  }
}

void PlayerController::_handle_ability_key(const String& action_name) {
  if (!keybind_map.has(action_name)) return;

  int slot = keybind_map[action_name];
  if (slot < 0 || slot > 5) return;

  // Cancel any existing targeting
  if (targeting_handler != nullptr) {
    targeting_handler->cancel();
  }

  CastingMode casting_mode = GameSettings::get_casting_mode_enum();

  switch (casting_mode) {
    case CastingMode::INSTANT: {
      // Cast immediately at cursor position
      if (cursor_query->has_hit()) {
        controlled_unit->relay(cast_ability, slot,
                               cursor_query->get_hit_object(),
                               cursor_query->get_hit_position());
      }
      break;
    }

    case CastingMode::CLICK_TO_CAST: {
      // Enter targeting mode
      if (targeting_handler != nullptr) {
        targeting_handler->start(slot, controlled_unit);
      }
      break;
    }

    case CastingMode::INDICATOR: {
      // TODO: indicator mode (hold to charge, release to cast)
      break;
    }
  }
}

void PlayerController::_handle_move_click() {
  if (!cursor_query->has_hit()) return;

  Unit* target_unit = cursor_query->get_hit_unit();

  if (target_unit != nullptr) {
    if (target_unit == controlled_unit) return;  // Ignore self-click

    // Same faction: ignore
    if (target_unit->get_faction_id() == controlled_unit->get_faction_id())
      return;

    // Enemy: attack
    controlled_unit->relay(attack_requested, target_unit,
                           target_unit->get_global_position());
    return;
  }

  // Terrain click: move
  Vector3 pos = cursor_query->get_hit_position();
  controlled_unit->relay(move_requested, pos);

  // Show click marker
  if (click_marker != nullptr) {
    click_marker->show_at(pos);
  }
}

void PlayerController::_handle_stop() {
  if (targeting_handler != nullptr) {
    targeting_handler->cancel();
  }
  controlled_unit->relay(stop_requested);
}

void PlayerController::_init_default_keybinds() {
  for (int i = 0; i < 6; i++) {
    String action_name = String("game_ability_") + String::num(i + 1, 0);
    keybind_map[action_name] = i;
  }
}

void PlayerController::set_controlled_unit(Unit* unit) {
  controlled_unit = unit;
}
Unit* PlayerController::get_controlled_unit() const {
  return controlled_unit;
}

void PlayerController::set_move_action(const String& action) {
  move_action = action;
}
String PlayerController::get_move_action() const { return move_action; }

void PlayerController::set_cast_action(const String& action) {
  cast_action = action;
}
String PlayerController::get_cast_action() const { return cast_action; }
