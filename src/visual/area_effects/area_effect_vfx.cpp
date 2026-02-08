#include "area_effect_vfx.hpp"

#include <godot_cpp/core/class_db.hpp>

#include "../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;

AreaEffectVFX::AreaEffectVFX() = default;

AreaEffectVFX::~AreaEffectVFX() = default;

void AreaEffectVFX::_bind_methods() {
  // Inherit from VFXNode, no additional methods needed
}

void AreaEffectVFX::play(const Dictionary& params) {
  DBG_INFO("AreaEffectVFX", "Playing area effect VFX");

  // Extract parameters
  effect_position = params.get("position", Vector3(0, 0, 0));
  effect_radius = params.get("radius", 1.0f);
  effect_duration = params.get("duration", 1.0f);

  // Set position
  set_global_position(effect_position);

  // Scale to match radius (assuming default scale represents 1.0 radius)
  set_scale(Vector3(effect_radius, effect_radius, effect_radius));

  // Mark as playing
  is_playing_internal = true;

  // NOTE: Tweens for fade/effects are best implemented in GDScript
  // where AnimationPlayer can handle the visual transitions
  set_duration(effect_duration);
}
