#include "explosion_vfx.hpp"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/color.hpp>

#include "../../debug/debug_macros.hpp"

using godot::AABB;
using godot::Callable;
using godot::ClassDB;
using godot::Color;
using godot::D_METHOD;
using godot::MeshInstance3D;

ExplosionVFX::ExplosionVFX() = default;

ExplosionVFX::~ExplosionVFX() = default;

void ExplosionVFX::_bind_methods() {
  ClassDB::bind_method(D_METHOD("_on_explosion_damage_signal"),
                       &ExplosionVFX::_on_explosion_damage_signal);
}

void ExplosionVFX::play(const Dictionary& params) {
  // Extract parameters
  explosion_position = params.get("position", Vector3(0, 0, 0));
  explosion_scale = params.get("scale", 1.0f);
  explosion_intensity = params.get("intensity", 1.0f);
  effect_duration = params.get("duration", 1.0f);

  // Extract animation-driven callback system parameters
  if (params.has("expected_signals")) {
    expected_signals = params["expected_signals"];
  }

  // Set position
  set_global_position(explosion_position);

  // Set initial scale
  set_scale(Vector3(explosion_scale, explosion_scale, explosion_scale));

  // Validate mesh size vs hitbox (explosion_scale represents the hitbox radius)
  _validate_mesh_size();

  // Mark as playing
  is_playing_internal = true;

  // Validate that animation will emit expected signals
  validate_animation_signals();

  // Get animation name from params or use default
  godot::String animation_name =
      params.get("animation_name", "explosion_anticipation_boom");

  // Start the animation
  AnimationPlayer* ap = get_animation_player();
  if (ap != nullptr) {
    // Create and connect expected signals to callback handler
    for (int i = 0; i < expected_signals.size(); i++) {
      godot::String signal_name = expected_signals[i];

      // Create the signal dynamically if it doesn't exist
      if (!has_signal(signal_name)) {
        add_user_signal(signal_name);
      }

      // Connect dynamic signal to appropriate handler
      // Use the parameterless handler instead of _on_animation_signal
      if (signal_name == "explosion_damage") {
        connect(signal_name, Callable(this, "_on_explosion_damage_signal"));
      }
    }

    ap->play(animation_name);
  }

  set_duration(effect_duration);
}

void ExplosionVFX::_validate_mesh_size() {
  // Find MeshInstance3D child to check its AABB
  MeshInstance3D* mesh_instance = nullptr;
  for (int i = 0; i < get_child_count(); i++) {
    mesh_instance = godot::Object::cast_to<MeshInstance3D>(get_child(i));
    if (mesh_instance != nullptr) {
      break;
    }
  }

  if (mesh_instance == nullptr) {
    DBG_WARN("ExplosionVFX",
             "No MeshInstance3D child found for AABB validation");
    return;
  }

  // Get the mesh's AABB in local space
  AABB mesh_aabb = mesh_instance->get_aabb();

  // Calculate the mesh's extents (half-size from center to corner)
  Vector3 mesh_extents = mesh_aabb.get_size() / 2.0f;
  float mesh_radius =
      mesh_extents.length();  // Diagonal distance from center to corner

  // The hitbox is a sphere with radius = explosion_scale
  float hitbox_radius = explosion_scale;

  // Warn if mesh extends beyond the hitbox
  if (mesh_radius > hitbox_radius) {
    DBG_WARN("ExplosionVFX",
             "Mesh radius (" + godot::String::num(mesh_radius, 2) +
                 ") exceeds hitbox radius (" +
                 godot::String::num(hitbox_radius, 2) +
                 "). Visual extends beyond collision area. "
                 "Increase 'scale' parameter or reduce mesh size.");
  }
}

void ExplosionVFX::_on_explosion_damage_signal() {
  _on_animation_signal("explosion_damage");
}

void ExplosionVFX::_on_finished() {
  // Disconnect explosion_damage signal before cleanup to prevent connection
  // leaks
  if (is_connected("explosion_damage",
                   Callable(this, "_on_explosion_damage_signal"))) {
    disconnect("explosion_damage",
               Callable(this, "_on_explosion_damage_signal"));
  }

  // Call parent cleanup (which handles child node cleanup and queue_free)
  VFXNode::_on_finished();
}
