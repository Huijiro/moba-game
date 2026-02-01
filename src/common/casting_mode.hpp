#ifndef GDEXTENSION_CASTING_MODE_H
#define GDEXTENSION_CASTING_MODE_H

/// Casting mode determines how abilities are cast when bound keys are pressed
enum class CastingMode {
  // Instant cast - ability executes immediately when key is pressed
  // Best for: Quick reactions, fast-paced gameplay
  INSTANT = 0,

  // Click to cast - press key to enter targeting mode, click to confirm cast
  // Best for: Skillshot abilities, precision targeting
  CLICK_TO_CAST = 1,

  // Indicator cast - press and hold key to show indicator, release to cast
  // Best for: Area abilities, directional abilities
  INDICATOR = 2,
};

#endif  // GDEXTENSION_CASTING_MODE_H
