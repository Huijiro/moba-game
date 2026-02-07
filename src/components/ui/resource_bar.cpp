#include "resource_bar.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/string.hpp>

#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"
#include "../resources/resource_pool_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::PropertyInfo;
using godot::String;
using godot::Variant;

ResourceBar::ResourceBar() = default;

ResourceBar::~ResourceBar() = default;

void ResourceBar::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_pool_id", "id"),
                       &ResourceBar::set_pool_id);
  ClassDB::bind_method(D_METHOD("get_pool_id"), &ResourceBar::get_pool_id);
  ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "pool_id"), "set_pool_id",
               "get_pool_id");

  ClassDB::bind_method(D_METHOD("_on_resource_changed", "current", "max"),
                       &ResourceBar::_on_resource_changed);
}

void ResourceBar::_ready() {
  if (Engine::get_singleton()->is_editor_hint()) {
    return;
  }

  // Find parent Unit
  Node* parent = get_parent();
  Unit* owner_unit = nullptr;

  // Walk up the tree to find the Unit
  while (parent) {
    owner_unit = Object::cast_to<Unit>(parent);
    if (owner_unit) {
      break;
    }
    parent = parent->get_parent();
  }

  if (!owner_unit) {
    DBG_WARN("ResourceBar", "No Unit parent found");
    return;
  }

  // Find ResourcePoolComponent with matching pool_id
  for (int i = 0; i < owner_unit->get_child_count(); i++) {
    ResourcePoolComponent* pool =
        Object::cast_to<ResourcePoolComponent>(owner_unit->get_child(i));
    if (pool && pool->get_pool_id() == pool_id) {
      resource_pool = pool;

      // Connect to value_changed signal
      pool->connect(
          godot::StringName("value_changed"),
          godot::Callable(this, godot::StringName("_on_resource_changed")));

      // Initialize the bar with current values
      set_max(pool->get_max_value());
      set_value(pool->get_current_value());

      DBG_DEBUG("ResourceBar",
                "Connected to ResourcePoolComponent[" + String(pool_id) + "]");
      return;
    }
  }

  DBG_WARN("ResourceBar", "ResourcePoolComponent with pool_id '" +
                              String(pool_id) + "' not found in Unit");
}

void ResourceBar::_on_resource_changed(float current, float max) {
  set_value(current);
  set_max(max);
}

void ResourceBar::set_pool_id(StringName id) {
  pool_id = id;
}

StringName ResourceBar::get_pool_id() const {
  return pool_id;
}
