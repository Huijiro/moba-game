#ifndef GDEXTENSION_ABILITY_EFFECT_MODULE_H
#define GDEXTENSION_ABILITY_EFFECT_MODULE_H

#include "ability_module.hpp"

/// Base class for modules that apply gameplay effects during execution.
class AbilityEffectModule : public AbilityModule {
  GDCLASS(AbilityEffectModule, AbilityModule)

 protected:
  static void _bind_methods();

 public:
  AbilityEffectModule();
  ~AbilityEffectModule();
};

#endif  // GDEXTENSION_ABILITY_EFFECT_MODULE_H
