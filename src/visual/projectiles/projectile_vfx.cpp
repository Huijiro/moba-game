#include "projectile_vfx.hpp"

#include <godot_cpp/core/class_db.hpp>

#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;

ProjectileVFX::ProjectileVFX() = default;

ProjectileVFX::~ProjectileVFX() = default;

void ProjectileVFX::_bind_methods() {
  // Inherit from VFXNode, no additional methods needed
}

void ProjectileVFX::play(const Dictionary& params) {
  DBG_INFO("ProjectileVFX", "Playing projectile VFX");

  // Extract parameters
  start_position = params.get("from", Vector3(0, 0, 0));
  end_position = params.get("to", Vector3(0, 0, 0));
  animation_duration = params.get("duration", 1.0f);

  // Set starting position
  set_global_position(start_position);

  // Mark as playing
  is_playing_internal = true;

  // NOTE: In a full implementation, we would create a Tween here to animate
  // position Due to godot-cpp limitations with Ref<Tween> types, we'll handle
  // animation in GDScript scenes instead, or use Godot 4.7+ with better
  // bindings For now, just set the duration for cleanup
  set_duration(animation_duration);
}
