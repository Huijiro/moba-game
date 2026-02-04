#ifndef GDEXTENSION_ORDER_UTILS_H
#define GDEXTENSION_ORDER_UTILS_H

#include <godot_cpp/variant/string.hpp>
#include "unit_order.hpp"

using godot::String;

/// Convert UnitOrder enum to human-readable string
inline String unit_order_to_string(UnitOrder order) {
  switch (order) {
    case UnitOrder::MOVE:
      return "MOVE";
    case UnitOrder::ATTACK:
      return "ATTACK";
    case UnitOrder::CHASE:
      return "CHASE";
    case UnitOrder::INTERACT:
      return "INTERACT";
    case UnitOrder::NONE:
      return "NONE";
    default:
      return "UNKNOWN";
  }
}

#endif  // GDEXTENSION_ORDER_UTILS_H
