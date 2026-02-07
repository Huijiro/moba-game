#include "projectile_vfx.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
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

  elapsed_time += static_cast<float>(delta);

  // Interpolate position based on elapsed time
  if (animation_duration > 0.0f) {
    float progress =
        godot::Math::clamp(elapsed_time / animation_duration, 0.0f, 1.0f);
    Vector3 current_position = start_position.lerp(end_position, progress);
    set_global_position(current_position);
  }

  // Call parent's process for duration checking and cleanup
  VFXNode::_process(delta);
}

void ProjectileVFX::play(const Dictionary& params) {
  DBG_INFO("ProjectileVFX", "Playing projectile VFX");

  // Extract parameters
  start_position = params.get("from", Vector3(0, 0, 0));
  end_position = params.get("to", Vector3(0, 0, 0));
  animation_duration = params.get("duration", 1.0f);

  // Set starting position
  set_global_position(start_position);
  elapsed_time = 0.0f;

  // Mark as playing
  is_playing_internal = true;

  // Set duration for auto-cleanup
  set_duration(animation_duration);

  DBG_INFO("ProjectileVFX", "Animating from (" +
                                String::num(start_position.x, 2) + ", " +
                                String::num(start_position.y, 2) + ", " +
                                String::num(start_position.z, 2) + ") to (" +
                                String::num(end_position.x, 2) + ", " +
                                String::num(end_position.y, 2) + ", " +
                                String::num(end_position.z, 2) + ") over " +
                                String::num(animation_duration, 2) + "s");
}
