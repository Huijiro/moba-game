#include "main_health_display.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

#include "../../core/match_manager.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../health/health_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::String;
using godot::VBoxContainer;

MainHealthDisplay::MainHealthDisplay() = default;

MainHealthDisplay::~MainHealthDisplay() = default;

void MainHealthDisplay::_bind_methods() {
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
  Unit* main_unit = match_manager->get_main_unit();
  if (!main_unit) {
    DBG_WARN("MainHealthDisplay", "No main_unit in MatchManager");
    return;
  }

  // Get HealthComponent
  health_component = Object::cast_to<HealthComponent>(
      main_unit->get_component_by_class("HealthComponent"));
  if (!health_component) {
    DBG_WARN("MainHealthDisplay", "No HealthComponent on main_unit");
    return;
  }

  // Create a VBoxContainer to hold the bar and label
  VBoxContainer* container = memnew(VBoxContainer);
  add_child(container);

  // Create the ProgressBar
  health_bar = memnew(ProgressBar);
  container->add_child(health_bar);
  health_bar->set_show_percentage(false);

  // Create the Label
  health_label = memnew(Label);
  container->add_child(health_label);
  health_label->set_text("0/0");

  // Connect to health signal
  health_component->connect(
      "health_changed",
      godot::Callable(this, godot::StringName("_on_health_changed")));

  // Initialize display with current health
  _on_health_changed(health_component->get_current_health(),
                     health_component->get_max_health());

  DBG_INFO("MainHealthDisplay", "Initialized for main unit");
}

void MainHealthDisplay::_on_health_changed(float current, float max) {
  if (health_bar) {
    health_bar->set_value(current);
    health_bar->set_max(max);
  }

  if (health_label) {
    String health_text =
        String::num((int)current) + " / " + String::num((int)max);
    health_label->set_text(health_text);
  }
}
