#include "projectile_vfx.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Node3D;
using godot::Object;
using godot::String;

ProjectileVFX::ProjectileVFX() = default;

ProjectileVFX::~ProjectileVFX() = default;

void ProjectileVFX::_bind_methods() {
  // Inherit from VFXNode, no additional methods needed
}

void ProjectileVFX::_process(double delta) {
  if (!is_playing_internal) {
    return;
  }

  // Check if tracked projectile still exists
  if (tracked_projectile != nullptr &&
      tracked_projectile->is_queued_for_deletion()) {
    // Projectile is being deleted, clean up this VFX
    DBG_INFO("ProjectileVFX", "Projectile destroyed, cleaning up VFX");
    _on_finished();
    return;
  }

  // If we have a projectile, mirror its position
  if (tracked_projectile != nullptr) {
    Node3D* projectile_node = Object::cast_to<Node3D>(tracked_projectile);
    if (projectile_node != nullptr && projectile_node->is_inside_tree()) {
      set_global_position(projectile_node->get_global_position());
      // Optionally match rotation too for better visuals
      set_global_rotation(projectile_node->get_global_rotation());
    }
  }

  // Don't call parent's _process - we handle our own lifecycle
  // (no duration-based cleanup, only follows projectile)
}

void ProjectileVFX::play(const Dictionary& params) {
  DBG_INFO("ProjectileVFX", "Starting projectile VFX");

  // Extract the projectile node to follow
  tracked_projectile = params.get("projectile", nullptr);
  if (tracked_projectile == nullptr) {
    DBG_WARN("ProjectileVFX", "No projectile provided in params");
    return;
  }

  Node3D* projectile_node = Object::cast_to<Node3D>(tracked_projectile);
  if (projectile_node == nullptr) {
    DBG_WARN("ProjectileVFX", "Projectile param is not a Node3D");
    tracked_projectile = nullptr;
    return;
  }

  // Mark as playing
  is_playing_internal = true;

  // Set initial position to match projectile
  set_global_position(projectile_node->get_global_position());

  DBG_INFO("ProjectileVFX",
           "Following projectile: " + projectile_node->get_name() +
               " at position (" +
               String::num(projectile_node->get_global_position().x, 2) + ", " +
               String::num(projectile_node->get_global_position().y, 2) + ", " +
               String::num(projectile_node->get_global_position().z, 2) + ")");
}
