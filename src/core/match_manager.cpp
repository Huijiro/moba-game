#include "match_manager.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../camera/moba_camera.hpp"
#include "../input/cursor_world_query.hpp"
#include "../input/hover_highlight.hpp"
#include "../input/player_controller.hpp"
#include "../input/targeting_preview.hpp"
#include "unit.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::Object;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

MatchManager::MatchManager() = default;

MatchManager::~MatchManager() = default;

void MatchManager::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_main_unit", "unit"),
                       &MatchManager::set_main_unit);
  ClassDB::bind_method(D_METHOD("get_main_unit"), &MatchManager::get_main_unit);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "main_unit",
                            godot::PROPERTY_HINT_NODE_TYPE, "Unit"),
               "set_main_unit", "get_main_unit");

  ClassDB::bind_method(D_METHOD("set_player_input", "input_root"),
                       &MatchManager::set_player_input);
  ClassDB::bind_method(D_METHOD("get_player_input"),
                       &MatchManager::get_player_input);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "player_input",
                            godot::PROPERTY_HINT_NODE_TYPE, "Node"),
               "set_player_input", "get_player_input");

  ClassDB::bind_method(D_METHOD("set_moba_camera", "camera"),
                       &MatchManager::set_moba_camera);
  ClassDB::bind_method(D_METHOD("get_moba_camera"),
                       &MatchManager::get_moba_camera);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "moba_camera",
                            godot::PROPERTY_HINT_NODE_TYPE, "MOBACamera"),
               "set_moba_camera", "get_moba_camera");
}

void MatchManager::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) return;

  if (main_unit == nullptr) {
    UtilityFunctions::push_warning("[MatchManager] main_unit is not set.");
    return;
  }

  if (player_input != nullptr) {
    _setup_player_input();
  } else {
    UtilityFunctions::push_warning(
        "[MatchManager] player_input is not set.");
  }

  if (moba_camera == nullptr) {
    UtilityFunctions::push_warning("[MatchManager] moba_camera is not set.");
  } else {
    moba_camera->set_target(main_unit);
  }
}

void MatchManager::_setup_player_input() {
  // Walk children of the player_input root and set controlled_unit
  for (int i = 0; i < player_input->get_child_count(); i++) {
    godot::Node* child = player_input->get_child(i);

    auto* controller = Object::cast_to<PlayerController>(child);
    if (controller != nullptr) {
      controller->set_controlled_unit(main_unit);
      continue;
    }

    auto* cursor = Object::cast_to<CursorWorldQuery>(child);
    if (cursor != nullptr) {
      cursor->set_controlled_unit(main_unit);
      continue;
    }

    auto* hover = Object::cast_to<HoverHighlight>(child);
    if (hover != nullptr) {
      hover->set_controlled_unit(main_unit);
      continue;
    }

    auto* preview = Object::cast_to<TargetingPreview>(child);
    if (preview != nullptr) {
      preview->set_controlled_unit(main_unit);
      continue;
    }
  }
}

void MatchManager::set_main_unit(Unit* unit) { main_unit = unit; }
Unit* MatchManager::get_main_unit() const { return main_unit; }

void MatchManager::set_player_input(Node* input_root) {
  player_input = input_root;
}
Node* MatchManager::get_player_input() const { return player_input; }

void MatchManager::set_moba_camera(MOBACamera* camera) { moba_camera = camera; }
MOBACamera* MatchManager::get_moba_camera() const { return moba_camera; }
