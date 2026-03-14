#ifndef GDEXTENSION_ABILITY_MODULE_H
#define GDEXTENSION_ABILITY_MODULE_H

#include "../ability_subcomponent.hpp"

/// Transitional base class for composable ability modules.
///
/// This now inherits from AbilitySubcomponent so the ability composition layer
/// mirrors the Unit + UnitComponent architecture. Existing module-oriented
/// classes can continue to derive from AbilityModule while the system evolves
/// toward ability-component terminology.
class AbilityModule : public AbilitySubcomponent {
  GDCLASS(AbilityModule, AbilitySubcomponent)

 protected:
  static void _bind_methods();

 public:
  AbilityModule();
  ~AbilityModule();
};

#endif  // GDEXTENSION_ABILITY_MODULE_H
