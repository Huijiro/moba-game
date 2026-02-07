#include "head_bar.hpp"

#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../health/health_component.hpp"
#include "../resources/resource_pool_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::String;
using godot::Variant;

HeadBar::HeadBar() = default;

HeadBar::~HeadBar() = default;

void HeadBar::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_unit_name_label_path", "path"),
                       &HeadBar::set_unit_name_label_path);
  ClassDB::bind_method(D_METHOD("get_unit_name_label_path"),
                       &HeadBar::get_unit_name_label_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "unit_name_label_path"),
               "set_unit_name_label_path", "get_unit_name_label_path");

  ClassDB::bind_method(D_METHOD("set_health_bar_path", "path"),
                       &HeadBar::set_health_bar_path);
  ClassDB::bind_method(D_METHOD("get_health_bar_path"),
                       &HeadBar::get_health_bar_path);
  ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "health_bar_path"),
               "set_health_bar_path", "get_health_bar_path");

  // Signal handlers
  ClassDB::bind_method(D_METHOD("_on_health_changed", "current", "max"),
                       &HeadBar::_on_health_changed);
  ClassDB::bind_method(
      D_METHOD("_on_resource_changed", "pool_id", "current", "max"),
      &HeadBar::_on_resource_changed);
  ClassDB::bind_method(D_METHOD("_reparent_to_game_ui", "game_ui"),
                       &HeadBar::_reparent_to_game_ui);
}

void HeadBar::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // 1. Find parent Unit
  Node* parent = get_parent();
  owner_unit = Object::cast_to<Unit>(parent);
  if (!owner_unit) {
    DBG_WARN("HeadBar", "Parent is not a Unit");
    return;
  }

  // 2. Find GameUI CanvasLayer and reparent to it
  godot::CanvasLayer* game_ui = nullptr;
  Node* current = parent;
  while (current) {
    game_ui = Object::cast_to<godot::CanvasLayer>(
        current->get_node_or_null("GameUI"));
    if (game_ui) {
      break;
    }
    current = current->get_parent();
  }

  if (game_ui) {
    // Defer reparenting to avoid "busy adding/removing children" error
    call_deferred(godot::StringName("_reparent_to_game_ui"),
                  godot::Variant(game_ui));
    DBG_DEBUG("HeadBar", "Scheduled reparent to GameUI CanvasLayer");
  } else {
    DBG_WARN("HeadBar", "GameUI CanvasLayer not found in scene");
    return;
  }

  // 4. Get camera from viewport
  auto viewport = get_viewport();
  if (viewport) {
    camera = viewport->get_camera_3d();
  }

  // 5. Get HealthComponent from parent
  health_component = Object::cast_to<HealthComponent>(
      owner_unit->get_component_by_class("HealthComponent"));
  if (health_component) {
    health_component->connect(
        "health_changed",
        godot::Callable(this, godot::StringName("_on_health_changed")));
    DBG_DEBUG("HeadBar", "Connected to HealthComponent.health_changed");
  } else {
    DBG_WARN("HeadBar", "No HealthComponent found on parent Unit");
  }

  // 6. Find all ResourcePoolComponents on Unit and discover matching child bars
  for (int i = 0; i < owner_unit->get_child_count(); i++) {
    ResourcePoolComponent* pool =
        Object::cast_to<ResourcePoolComponent>(owner_unit->get_child(i));
    if (!pool) {
      continue;
    }

    StringName pool_id = pool->get_pool_id();
    String child_name = "Resource" + String(pool_id);

    // Look for matching child ProgressBar
    ProgressBar* bar = Object::cast_to<ProgressBar>(
        get_node_or_null(godot::NodePath(child_name)));

    if (bar) {
      // Found matching bar - connect and track
      resource_bars.push_back(bar);
      resource_pool_ids.push_back(pool_id);
      resource_pools.push_back(pool);

      // Connect to value_changed signal with pool_id parameter
      pool->connect(
          "value_changed",
          godot::Callable(this, godot::StringName("_on_resource_changed"))
              .bindv(godot::Array::make(pool_id)));

      DBG_DEBUG("HeadBar",
                "Connected to ResourcePoolComponent[" + String(pool_id) + "]");
    }
    // If not found: gracefully skip (no error)
  }

  // 7. Find unit name label and set text
  unit_name_label =
      Object::cast_to<Label>(get_node_or_null(unit_name_label_path));
  if (unit_name_label) {
    unit_name_label->set_text(owner_unit->get_name());
    DBG_DEBUG("HeadBar", "Set unit name to: " + owner_unit->get_name());
  } else {
    DBG_WARN("HeadBar", "UnitNameLabel not found at path: " +
                            String(unit_name_label_path));
  }

  // 8. Find health bar
  health_bar = Object::cast_to<ProgressBar>(get_node_or_null(health_bar_path));
  if (!health_bar) {
    DBG_WARN("HeadBar",
             "HealthBar not found at path: " + String(health_bar_path));
  }

  DBG_INFO("HeadBar", "Initialized for unit: " + owner_unit->get_name());
}

void HeadBar::_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  if (!owner_unit || !camera) {
    return;
  }

  // Find MeshInstance3D to get AABB for positioning
  godot::MeshInstance3D* mesh_instance = nullptr;
  for (int i = 0; i < owner_unit->get_child_count(); i++) {
    mesh_instance =
        Object::cast_to<godot::MeshInstance3D>(owner_unit->get_child(i));
    if (mesh_instance) {
      break;
    }
  }

  Vector3 unit_pos = owner_unit->get_global_position();
  Vector3 world_pos =
      unit_pos + Vector3(0, 3, 0);  // Default position above unit

  // If we found a mesh, use its AABB to position more precisely
  if (mesh_instance) {
    godot::AABB aabb = mesh_instance->get_aabb();
    Vector3 aabb_center = aabb.get_center();

    // Position above the top of the AABB, centered on X/Z
    // The mesh instance has a transform, so we need to account for that
    Vector3 mesh_offset = mesh_instance->get_position();
    Vector3 aabb_top_center =
        mesh_offset + Vector3(aabb_center.x, aabb.size.y / 2.0f, aabb_center.z);
    world_pos = unit_pos + aabb_top_center + Vector3(0, 0.5f, 0);
  }

  // Convert to screen position
  godot::Vector2 screen_pos = camera->unproject_position(world_pos);
  set_global_position(screen_pos);
}

void HeadBar::_on_health_changed(float current, float max) {
  if (!health_bar) {
    return;
  }

  health_bar->set_value(current);
  health_bar->set_max(max);
}

void HeadBar::_on_resource_changed(StringName pool_id,
                                   float current,
                                   float max) {
  // Find which bar corresponds to this pool
  for (size_t i = 0; i < resource_pool_ids.size(); i++) {
    if (resource_pool_ids[i] == pool_id && i < resource_bars.size()) {
      ProgressBar* bar = resource_bars[i];
      bar->set_value(current);
      bar->set_max(max);
      return;
    }
  }
}

void HeadBar::set_unit_name_label_path(const godot::NodePath& path) {
  unit_name_label_path = path;
}

godot::NodePath HeadBar::get_unit_name_label_path() const {
  return unit_name_label_path;
}

void HeadBar::set_health_bar_path(const godot::NodePath& path) {
  health_bar_path = path;
}

godot::NodePath HeadBar::get_health_bar_path() const {
  return health_bar_path;
}

void HeadBar::_reparent_to_game_ui(godot::Object* game_ui) {
  if (!game_ui || !is_inside_tree()) {
    DBG_WARN("HeadBar", "_reparent_to_game_ui: Invalid game_ui or not in tree");
    return;
  }

  godot::CanvasLayer* canvas_layer =
      Object::cast_to<godot::CanvasLayer>(game_ui);
  if (!canvas_layer) {
    DBG_WARN("HeadBar", "_reparent_to_game_ui: game_ui is not a CanvasLayer");
    return;
  }

  Node* parent = get_parent();
  if (parent) {
    parent->remove_child(this);
  }
  canvas_layer->add_child(this);
  DBG_DEBUG("HeadBar", "Successfully reparented to GameUI CanvasLayer");
}
