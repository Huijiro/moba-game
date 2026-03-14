#ifndef GDEXTENSION_ABILITY_TARGETING_MODULE_H
#define GDEXTENSION_ABILITY_TARGETING_MODULE_H

#include "ability_module.hpp"

/// Base class for target selection/validation modules.
class AbilityTargetingModule : public AbilityModule {
  GDCLASS(AbilityTargetingModule, AbilityModule)

 protected:
  static void _bind_methods();

 public:
  AbilityTargetingModule();
  ~AbilityTargetingModule();

  virtual int get_targeting_type() const;
  virtual bool requires_unit_target() const;
  virtual bool requires_point_target() const;
};

#endif  // GDEXTENSION_ABILITY_TARGETING_MODULE_H
