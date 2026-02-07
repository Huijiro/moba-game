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
  // Inherit from VFXNode, no additional methods needed
}

void ExplosionVFX::play(const Dictionary& params) {
  DBG_INFO("ExplosionVFX", "Playing explosion VFX");

  // Extract parameters
  explosion_position = params.get("position", Vector3(0, 0, 0));
  explosion_scale = params.get("scale", 1.0f);
  explosion_intensity = params.get("intensity", 1.0f);
  effect_duration = params.get("duration", 1.0f);

  // Extract animation-driven callback system parameters
  if (params.has("expected_signals")) {
    expected_signals = params["expected_signals"];
    DBG_INFO("ExplosionVFX", "Expected signals: " +
                                 godot::String::num(expected_signals.size()));
  }

  // Register callbacks for animation signals
  // Check for each expected signal and register its callback
  for (int i = 0; i < expected_signals.size(); i++) {
    godot::String signal_name = expected_signals[i];
    if (params.has(signal_name)) {
      // Callback is stored in params - ability provides it
      DBG_INFO("ExplosionVFX", "Found callback for signal: " + signal_name);
    }
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
    DBG_INFO("ExplosionVFX", "Found AnimationPlayer, connecting signals...");

    // Connect expected signals to callback handler
    DBG_INFO("ExplosionVFX", "Expected signals count: " +
                                 godot::String::num(expected_signals.size()));

    for (int i = 0; i < expected_signals.size(); i++) {
      godot::String signal_name = expected_signals[i];
      DBG_INFO("ExplosionVFX", "[Signal " + godot::String::num(i) + "] " +
                                   signal_name + " - connecting...");

      // Connect dynamic signal to _on_animation_signal handler
      connect(signal_name, Callable(this, "_on_animation_signal"));

      DBG_INFO("ExplosionVFX", "[Signal " + godot::String::num(i) + "] " +
                                   signal_name + " - CONNECTED");
    }

    DBG_INFO("ExplosionVFX",
             "All signals connected, starting animation: " + animation_name);

    // Verify animation exists before playing
    godot::String current = ap->get_assigned_animation();
    DBG_INFO("ExplosionVFX", "Current animation before play: " +
                                 (current.is_empty() ? "NONE" : current));

    ap->play(animation_name);

    // Verify animation started
    current = ap->get_assigned_animation();
    DBG_INFO("ExplosionVFX", "Current animation after play: " +
                                 (current.is_empty() ? "NONE" : current));
    DBG_INFO("ExplosionVFX",
             "Animation is playing: " +
                 godot::String(ap->is_playing() ? "YES" : "NO"));
    DBG_INFO("ExplosionVFX",
             "Animation length: " +
                 godot::String::num(ap->get_current_animation_length(), 2) +
                 "s");
  } else {
    DBG_WARN("ExplosionVFX", "No AnimationPlayer found for explosion VFX");
  }

  set_duration(effect_duration);
  DBG_INFO("ExplosionVFX",
           "VFX duration set to: " + godot::String::num(effect_duration));
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
