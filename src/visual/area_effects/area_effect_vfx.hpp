#ifndef GDEXTENSION_AREA_EFFECT_VFX_H
#define GDEXTENSION_AREA_EFFECT_VFX_H

#include <godot_cpp/variant/vector3.hpp>

#include "../vfx_node.hpp"

using godot::Vector3;

/// Area Effect VFX - Persistent ground effect for AoE abilities
/// Used for slow zones, status effect indicators, persistent damage areas
///
/// Parameters:
/// - "position": Vector3 - Center of effect
/// - "radius": float - Effect radius
/// - "duration": float - How long the effect persists
///
/// Example:
/// area_effect_vfx->play({
///   {"position", spell_center},
///   {"radius", 5.0f},
///   {"duration", 3.0f}
/// });
class AreaEffectVFX : public VFXNode {
  GDCLASS(AreaEffectVFX, VFXNode)

 protected:
  static void _bind_methods();

  // Animation parameters
  Vector3 effect_position = Vector3(0, 0, 0);
  float effect_radius = 1.0f;
  float effect_duration = 1.0f;

 public:
  AreaEffectVFX();
  ~AreaEffectVFX();

  // Display area effect with radius for specified duration
  void play(const Dictionary& params) override;
};

#endif  // GDEXTENSION_AREA_EFFECT_VFX_H
