#include "main_resource_display.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/string.hpp>

#include "../../core/match_manager.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../resources/resource_pool_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::String;
using godot::Variant;

MainResourceDisplay::MainResourceDisplay() = default;

MainResourceDisplay::~MainResourceDisplay() = default;

void MainResourceDisplay::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_pool_id", "id"),
                       &MainResourceDisplay::set_pool_id);
  ClassDB::bind_method(D_METHOD("get_pool_id"),
                       &MainResourceDisplay::get_pool_id);
  ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "pool_id"), "set_pool_id",
               "get_pool_id");

  ClassDB::bind_method(D_METHOD("set_resource_bar_path", "path"),
                       &MainResourceDisplay::set_resource_bar_path);
  ClassDB::bind_method(D_METHOD("get_resource_bar_path"),
                       &MainResourceDisplay::get_resource_bar_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "resource_bar_path"),
               "set_resource_bar_path", "get_resource_bar_path");

  ClassDB::bind_method(D_METHOD("set_resource_label_path", "path"),
                       &MainResourceDisplay::set_resource_label_path);
  ClassDB::bind_method(D_METHOD("get_resource_label_path"),
                       &MainResourceDisplay::get_resource_label_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "resource_label_path"),
               "set_resource_label_path", "get_resource_label_path");

  ClassDB::bind_method(D_METHOD("_on_resource_changed", "current", "max"),
                       &MainResourceDisplay::_on_resource_changed);
}

void MainResourceDisplay::_ready() {
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
    DBG_WARN("MainResourceDisplay", "MatchManager not found in scene");
    return;
  }

  // Get main unit
  Unit* main_unit = match_manager->get_main_unit();
  if (!main_unit) {
    DBG_WARN("MainResourceDisplay", "No main_unit in MatchManager");
    return;
  }

  // Find ResourcePoolComponent with matching pool_id
  for (int i = 0; i < main_unit->get_child_count(); i++) {
    ResourcePoolComponent* pool =
        Object::cast_to<ResourcePoolComponent>(main_unit->get_child(i));
    if (pool && pool->get_pool_id() == pool_id) {
      resource_pool = pool;
      break;
    }
  }

  if (!resource_pool) {
    DBG_WARN("MainResourceDisplay", "ResourcePoolComponent with pool_id '" +
                                        String(pool_id) + "' not found");
    return;
  }

  // Find child nodes
  resource_bar =
      Object::cast_to<ProgressBar>(get_node_or_null(resource_bar_path));
  if (!resource_bar) {
    DBG_WARN("MainResourceDisplay",
             "ResourceBar not found at path: " + String(resource_bar_path));
    return;
  }

  resource_label =
      Object::cast_to<Label>(get_node_or_null(resource_label_path));
  if (!resource_label) {
    DBG_WARN("MainResourceDisplay",
             "ResourceLabel not found at path: " + String(resource_label_path));
    return;
  }

  // Connect to resource signal
  resource_pool->connect(
      "value_changed",
      godot::Callable(this, godot::StringName("_on_resource_changed")));

  // Initialize display with current values
  _on_resource_changed(resource_pool->get_current_value(),
                       resource_pool->get_max_value());

  DBG_INFO("MainResourceDisplay",
           "Initialized for pool '" + String(pool_id) + "'");
}

void MainResourceDisplay::_on_resource_changed(float current, float max) {
  if (resource_bar) {
    resource_bar->set_value(current);
    resource_bar->set_max(max);
  }

  if (resource_label) {
    String resource_text =
        String::num((int)current) + " / " + String::num((int)max);
    resource_label->set_text(resource_text);
  }
}

void MainResourceDisplay::set_pool_id(StringName id) {
  pool_id = id;
}

StringName MainResourceDisplay::get_pool_id() const {
  return pool_id;
}

void MainResourceDisplay::set_resource_bar_path(godot::NodePath path) {
  resource_bar_path = path;
}

godot::NodePath MainResourceDisplay::get_resource_bar_path() const {
  return resource_bar_path;
}

void MainResourceDisplay::set_resource_label_path(godot::NodePath path) {
  resource_label_path = path;
}

godot::NodePath MainResourceDisplay::get_resource_label_path() const {
  return resource_label_path;
}
