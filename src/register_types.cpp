#include "register_types.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "ai/test_movement.hpp"
#include "camera/moba_camera.hpp"

#include "components/abilities/ability_component.hpp"
#include "components/abilities/ability_definition.hpp"
#include "components/abilities/ability_effect.hpp"
#include "components/abilities/effects/aoe_damage_effect.hpp"
#include "components/abilities/effects/damage_effect.hpp"
#include "components/abilities/effects/skillshot_effect.hpp"
#include "components/combat/attack_component.hpp"
#include "components/combat/projectile.hpp"
#include "components/combat/skillshot_projectile.hpp"
#include "components/health/health_component.hpp"
#include "components/interaction/interactable.hpp"
#include "components/movement/movement_component.hpp"
#include "components/resources/resource_pool_component.hpp"
#include "components/unit_component.hpp"
#include "core/match_manager.hpp"
#include "core/unit.hpp"
#include "debug/visual_debugger.hpp"
#include "input/input_manager.hpp"
#include "visual/projectile_visual.hpp"
#include "visual/visual_effect.hpp"

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  GDREGISTER_CLASS(Unit)
  GDREGISTER_CLASS(Interactable)
  GDREGISTER_CLASS(InputManager)
  GDREGISTER_CLASS(MOBACamera)
  GDREGISTER_CLASS(MatchManager)
  GDREGISTER_CLASS(TestMovement)
  GDREGISTER_CLASS(UnitComponent)
  GDREGISTER_CLASS(MovementComponent)
  GDREGISTER_CLASS(HealthComponent)
  GDREGISTER_CLASS(ResourcePoolComponent)
  GDREGISTER_CLASS(AttackComponent)
  GDREGISTER_CLASS(Projectile)
  GDREGISTER_CLASS(SkillshotProjectile)
  GDREGISTER_CLASS(AbilityEffect)
  GDREGISTER_CLASS(AbilityDefinition)
  GDREGISTER_CLASS(DamageEffect)
  GDREGISTER_CLASS(AoEDamageEffect)
  GDREGISTER_CLASS(SkillshotEffect)
  GDREGISTER_CLASS(AbilityComponent)
  GDREGISTER_CLASS(VisualDebugger)
  GDREGISTER_CLASS(VisualEffect)
  GDREGISTER_CLASS(ProjectileVisual)
}

void uninitialize_example_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C" {
// Initialization.
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
