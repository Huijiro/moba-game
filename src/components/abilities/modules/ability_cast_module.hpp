#ifndef GDEXTENSION_ABILITY_CAST_MODULE_H
#define GDEXTENSION_ABILITY_CAST_MODULE_H

#include "ability_module.hpp"

/// Base class for cast timing/profile modules.
class AbilityCastModule : public AbilityModule {
  GDCLASS(AbilityCastModule, AbilityModule)

 protected:
  static void _bind_methods();

 public:
  AbilityCastModule();
  ~AbilityCastModule();

  virtual int get_cast_type() const;
  virtual float get_cast_duration() const;
  virtual float get_cast_point() const;
  virtual float get_channel_duration() const;
  virtual float get_tick_interval() const;
};

#endif  // GDEXTENSION_ABILITY_CAST_MODULE_H
