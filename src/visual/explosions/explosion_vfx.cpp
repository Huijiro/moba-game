#include "explosion_vfx.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/color.hpp>

#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::Color;
using godot::D_METHOD;

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

  // Set position
  set_global_position(explosion_position);

  // Set initial scale
  set_scale(Vector3(explosion_scale, explosion_scale, explosion_scale));

  // Mark as playing
  is_playing_internal = true;

  // NOTE: In a full implementation, tweens would animate scale and modulate
  // Due to godot-cpp limitations, animations are best done in GDScript scenes
  // where VFX nodes can use their own AnimationPlayer or Tween setup
  set_duration(effect_duration);
}
