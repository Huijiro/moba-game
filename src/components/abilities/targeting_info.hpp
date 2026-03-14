#ifndef GDEXTENSION_TARGETING_INFO_H
#define GDEXTENSION_TARGETING_INFO_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

/// Lightweight data object passed through the "query_targeting" signal.
/// Targeting components fill in their data. AbilityNode doesn't know
/// about any component type — it just passes this bag through.
///
/// Targeting types:
///   NONE  (0) — passive or no targeting (not castable)
///   UNIT  (1) — needs a unit target
///   POINT (2) — needs a ground position
///   SELF  (3) — self-cast, no targeting needed
class TargetingInfo : public godot::RefCounted {
  GDCLASS(TargetingInfo, godot::RefCounted)

 protected:
  static void _bind_methods();

 public:
  enum TargetingType {
    NONE = 0,
    UNIT = 1,
    POINT = 2,
    SELF = 3,
  };

  TargetingInfo() = default;
  ~TargetingInfo() = default;

  void set_type(int t);
  int get_type() const;

  void set_range(float r);
  float get_range() const;

  void set_aoe_radius(float r);
  float get_aoe_radius() const;

 private:
  int targeting_type = NONE;
  float range = 0.0f;
  float aoe_radius = 0.0f;
};

VARIANT_ENUM_CAST(TargetingInfo::TargetingType);

#endif  // GDEXTENSION_TARGETING_INFO_H
