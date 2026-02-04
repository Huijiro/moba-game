#ifndef GDEXTENSION_SIGNAL_NAME_H
#define GDEXTENSION_SIGNAL_NAME_H

// Type alias for clarity - signals are defined as C++ strings
// They are converted to StringName at runtime to avoid static initialization
// issues
using SignalName = const char*;

#endif  // GDEXTENSION_SIGNAL_NAME_H
