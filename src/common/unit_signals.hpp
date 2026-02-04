#ifndef GDEXTENSION_UNIT_SIGNALS_H
#define GDEXTENSION_UNIT_SIGNALS_H

#include "signal_name.hpp"

// Movement signals - emitted by Unit relay from InputManager/AI
extern const SignalName move_requested;
extern const SignalName attack_requested;
extern const SignalName chase_requested;
extern const SignalName interact_requested;
extern const SignalName stop_requested;

// Damage signals - emitted by Unit relay from attack systems
extern const SignalName take_damage;

#endif  // GDEXTENSION_UNIT_SIGNALS_H
