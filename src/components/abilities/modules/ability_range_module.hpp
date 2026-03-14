#ifndef GDEXTENSION_ABILITY_RANGE_MODULE_H
#define GDEXTENSION_ABILITY_RANGE_MODULE_H

#include "ability_module.hpp"

/// Base class for range/position policy modules.
class AbilityRangeModule : public AbilityModule {
  GDCLASS(AbilityRangeModule, AbilityModule)

 protected:
  static void _bind_methods();

 public:
  AbilityRangeModule();
  ~AbilityRangeModule();

  virtual float get_range() const;
};

#endif  // GDEXTENSION_ABILITY_RANGE_MODULE_H
