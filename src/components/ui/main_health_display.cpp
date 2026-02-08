#include "main_health_display.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/string.hpp>

#include "../../common/unit_signals.hpp"
#include "../../core/match_manager.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../health/health_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::String;
using godot::Variant;

MainHealthDisplay::MainHealthDisplay() = default;

MainHealthDisplay::~MainHealthDisplay() = default;

void MainHealthDisplay::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_health_bar_path", "path"),
                       &MainHealthDisplay::set_health_bar_path);
  ClassDB::bind_method(D_METHOD("get_health_bar_path"),
                       &MainHealthDisplay::get_health_bar_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "health_bar_path"),
               "set_health_bar_path", "get_health_bar_path");

  ClassDB::bind_method(D_METHOD("set_health_label_path", "path"),
                       &MainHealthDisplay::set_health_label_path);
  ClassDB::bind_method(D_METHOD("get_health_label_path"),
                       &MainHealthDisplay::get_health_label_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "health_label_path"),
               "set_health_label_path", "get_health_label_path");

  ClassDB::bind_method(D_METHOD("_on_health_changed", "current", "max"),
                       &MainHealthDisplay::_on_health_changed);
}

void MainHealthDisplay::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Find MatchManager in scene root
  Node* current = this;
  Node* scene_root = nullptr;

  while (current && current->get_parent()) {
    scene_root = current;
    current = current->get_parent();
  }

  MatchManager* match_manager = nullptr;
  if (scene_root) {
    for (int i = 0; i < scene_root->get_child_count(); i++) {
      match_manager = Object::cast_to<MatchManager>(scene_root->get_child(i));
      if (match_manager) {
        break;
      }
    }
  }

  if (!match_manager) {
    DBG_WARN("MainHealthDisplay", "MatchManager not found in scene");
    return;
  }

  // Get main unit
  main_unit = match_manager->get_main_unit();
  if (!main_unit) {
    DBG_WARN("MainHealthDisplay", "No main_unit in MatchManager");
    return;
  }

  // Find child nodes
  health_bar = Object::cast_to<ProgressBar>(get_node_or_null(health_bar_path));
  if (!health_bar) {
    DBG_WARN("MainHealthDisplay",
             "HealthBar not found at path: " + String(health_bar_path));
    return;
  }

  health_label = Object::cast_to<Label>(get_node_or_null(health_label_path));
  if (!health_label) {
    DBG_WARN("MainHealthDisplay",
             "HealthLabel not found at path: " + String(health_label_path));
    return;
  }

  // Connect to Unit's health_changed signal (emitted by HealthComponent)
  main_unit->connect(
      health_changed,
      godot::Callable(this, godot::StringName("_on_health_changed")));

  // Get initial health values from HealthComponent to display on startup
  for (int i = 0; i < main_unit->get_child_count(); i++) {
    auto health_comp =
        Object::cast_to<HealthComponent>(main_unit->get_child(i));
    if (health_comp) {
      _on_health_changed(health_comp->get_current_health(),
                         health_comp->get_max_health());
      break;
    }
  }

  DBG_INFO("MainHealthDisplay", "Initialized for main unit");
}

void MainHealthDisplay::_on_health_changed(float current, float max) {
  if (health_bar) {
    health_bar->set_max(max);
    health_bar->set_value(current);
  }

  if (health_label) {
    String health_text =
        String::num((int)current) + " / " + String::num((int)max);
    health_label->set_text(health_text);
  }
}

void MainHealthDisplay::set_health_bar_path(godot::NodePath path) {
  health_bar_path = path;
}

godot::NodePath MainHealthDisplay::get_health_bar_path() const {
  return health_bar_path;
}

void MainHealthDisplay::set_health_label_path(godot::NodePath path) {
  health_label_path = path;
}

godot::NodePath MainHealthDisplay::get_health_label_path() const {
  return health_label_path;
}
