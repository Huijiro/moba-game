#include "register_types.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "ai/test_movement.hpp"
#include "camera/moba_camera.hpp"

// Ability system
#include "components/abilities/ability_component.hpp"
#include "components/abilities/ability_context.hpp"
#include "components/abilities/ability_node.hpp"
#include "components/abilities/ability_subcomponent.hpp"
#include "components/abilities/targeting_info.hpp"
#include "components/abilities/ability_components/aoe_damage_component.hpp"
#include "components/abilities/ability_components/cast_time_component.hpp"
#include "components/abilities/ability_components/channel_component.hpp"
#include "components/abilities/ability_components/cooldown_component.hpp"
#include "components/abilities/ability_components/damage_effect_component.hpp"
#include "components/abilities/ability_components/homing_projectile_component.hpp"
#include "components/abilities/ability_components/instant_cast_component.hpp"
#include "components/abilities/ability_components/point_targeting_component.hpp"
#include "components/abilities/ability_components/resource_cost_component.hpp"
#include "components/abilities/ability_components/skillshot_projectile_component.hpp"
#include "components/abilities/ability_components/unit_targeting_component.hpp"
#include "components/abilities/ability_components/vfx_component.hpp"

// Unit components
#include "components/combat/attack_component.hpp"
#include "components/combat/projectile.hpp"
#include "components/combat/skillshot_projectile.hpp"
#include "components/health/health_component.hpp"
#include "components/interaction/interactable.hpp"
#include "components/movement/movement_component.hpp"
#include "components/resources/resource_pool_component.hpp"
#include "components/revive/revive_component.hpp"
#include "components/ui/cooldown_display_component.hpp"
#include "components/ui/cooldown_icon.hpp"
#include "components/ui/head_bar.hpp"
#include "components/ui/label_component.hpp"
#include "components/ui/main_health_display.hpp"
#include "components/ui/main_resource_display.hpp"
#include "components/ui/resource_bar.hpp"
#include "components/unit_component.hpp"

// Core
#include "core/match_manager.hpp"
#include "core/unit.hpp"
#include "debug/debug_logger.hpp"
#include "debug/visual_debugger.hpp"
#include "input/input_manager.hpp"

// VFX (legacy — kept for now, no longer used by ability scenes)
#include "visual/area_effects/area_effect_vfx.hpp"
#include "visual/explosions/explosion_vfx.hpp"
#include "visual/projectiles/projectile_vfx.hpp"
#include "visual/vfx_node.hpp"

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  // Core
  GDREGISTER_CLASS(Unit)
  GDREGISTER_CLASS(InputManager)
  GDREGISTER_CLASS(MOBACamera)
  GDREGISTER_CLASS(MatchManager)
  GDREGISTER_CLASS(TestMovement)

  // Unit components
  GDREGISTER_CLASS(UnitComponent)
  GDREGISTER_CLASS(Interactable)
  GDREGISTER_CLASS(MovementComponent)
  GDREGISTER_CLASS(HealthComponent)
  GDREGISTER_CLASS(ReviveComponent)
  GDREGISTER_CLASS(ResourcePoolComponent)
  GDREGISTER_CLASS(AttackComponent)
  GDREGISTER_CLASS(Projectile)
  GDREGISTER_CLASS(SkillshotProjectile)

  // UI components
  GDREGISTER_CLASS(LabelComponent)
  GDREGISTER_CLASS(HeadBar)
  GDREGISTER_CLASS(ResourceBar)
  GDREGISTER_CLASS(CooldownIcon)
  GDREGISTER_CLASS(CooldownDisplayComponent)
  GDREGISTER_CLASS(MainHealthDisplay)
  GDREGISTER_CLASS(MainResourceDisplay)

  // Ability system
  GDREGISTER_CLASS(AbilityContext)
  GDREGISTER_CLASS(TargetingInfo)
  GDREGISTER_CLASS(AbilityNode)
  GDREGISTER_CLASS(AbilitySubcomponent)
  GDREGISTER_CLASS(AbilityComponent)
  GDREGISTER_CLASS(InstantCastComponent)
  GDREGISTER_CLASS(CastTimeComponent)
  GDREGISTER_CLASS(ChannelComponent)
  GDREGISTER_CLASS(UnitTargetingComponent)
  GDREGISTER_CLASS(PointTargetingComponent)
  GDREGISTER_CLASS(DamageEffectComponent)
  GDREGISTER_CLASS(AoEDamageComponent)
  GDREGISTER_CLASS(HomingProjectileComponent)
  GDREGISTER_CLASS(SkillshotProjectileComponent)
  GDREGISTER_CLASS(CooldownComponent)
  GDREGISTER_CLASS(ResourceCostComponent)
  GDREGISTER_CLASS(VFXComponent)

  // Debug
  GDREGISTER_CLASS(VisualDebugger)
  GDREGISTER_CLASS(DebugLogger)

  // VFX (legacy — kept for backward compatibility)
  GDREGISTER_CLASS(VFXNode)
  GDREGISTER_CLASS(ProjectileVFX)
  GDREGISTER_CLASS(ExplosionVFX)
  GDREGISTER_CLASS(AreaEffectVFX)
}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C" {
GDExtensionBool GDE_EXPORT
GDExtensionInit(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                const GDExtensionClassLibraryPtr p_library,
                GDExtensionInitialization* r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library,
                                                 r_initialization);

  init_obj.register_initializer(initialize_example_module);
  init_obj.register_terminator(uninitialize_example_module);
  init_obj.set_minimum_library_initialization_level(
      MODULE_INITIALIZATION_LEVEL_SCENE);

  return init_obj.init();
}
}
