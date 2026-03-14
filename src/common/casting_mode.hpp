#ifndef GDEXTENSION_CASTING_MODE_H
#define GDEXTENSION_CASTING_MODE_H

#include <godot_cpp/variant/string.hpp>

/// Casting mode determines how abilities are cast when bound keys are pressed
enum class CastingMode {
  // Instant cast - ability executes immediately when key is pressed
  INSTANT = 0,

  // Click to cast - press key to enter targeting mode, click to confirm cast
  CLICK_TO_CAST = 1,

  // Indicator cast - press and hold key to show indicator, release to cast
  INDICATOR = 2,
};

inline CastingMode casting_mode_from_string(const godot::String& str) {
  if (str == "instant") return CastingMode::INSTANT;
  if (str == "indicator") return CastingMode::INDICATOR;
  return CastingMode::CLICK_TO_CAST;  // default
}

inline godot::String casting_mode_to_string(CastingMode mode) {
  switch (mode) {
    case CastingMode::INSTANT:
      return "instant";
    case CastingMode::CLICK_TO_CAST:
      return "click_to_cast";
    case CastingMode::INDICATOR:
      return "indicator";
  }
  return "click_to_cast";
}

#endif  // GDEXTENSION_CASTING_MODE_H
