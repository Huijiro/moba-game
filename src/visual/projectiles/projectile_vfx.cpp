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

  // If we have a projectile, mirror its position (as backup in case reparenting
  // didn't work)
  if (tracked_projectile != nullptr) {
    Node3D* projectile_node = Object::cast_to<Node3D>(tracked_projectile);
    if (projectile_node != nullptr && projectile_node->is_inside_tree()) {
      // Only update position if we're in the tree and not already a child
      if (is_inside_tree() && get_parent() != projectile_node) {
        set_global_position(projectile_node->get_global_position());
        set_global_rotation(projectile_node->get_global_rotation());
      }
    } else if (projectile_node != nullptr &&
               !projectile_node->is_inside_tree()) {
      // Projectile is no longer in tree, clean up
      DBG_INFO("ProjectileVFX",
               "Projectile no longer in tree, cleaning up VFX");
      _on_finished();
      return;
    }
  }
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

  // Make this VFX a child of the projectile so they move together
  // This avoids any position synchronization issues
  if (get_parent() != projectile_node) {
    // Move this node to be a child of the projectile
    Node* current_parent = get_parent();
    if (current_parent != nullptr) {
      current_parent->remove_child(this);
    }
    projectile_node->add_child(this);
    DBG_INFO("ProjectileVFX",
             "Made VFX a child of projectile for synchronized movement");
  }

  DBG_INFO("ProjectileVFX",
           "Following projectile: " + projectile_node->get_name());
}
