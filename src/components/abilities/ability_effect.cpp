#include "ability_effect.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../core/unit.hpp"
#include "ability_definition.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Engine;
using godot::UtilityFunctions;

AbilityEffect::AbilityEffect() = default;

AbilityEffect::~AbilityEffect() = default;

void AbilityEffect::_bind_methods() {
  ClassDB::bind_method(D_METHOD("execute", "caster", "target", "ability"),
                       &AbilityEffect::execute);
  ClassDB::bind_method(
      D_METHOD("query_units_in_sphere", "center", "radius", "exclude_unit"),
      &AbilityEffect::query_units_in_sphere, nullptr);
}

void AbilityEffect::execute(Unit* caster,
                            Object* target,
                            const AbilityDefinition* ability) {
  // Base implementation does nothing - subclasses override
  UtilityFunctions::print(
      "[AbilityEffect] Base effect executed with no implementation");
}

void AbilityEffect::execute_at_point(Unit* caster,
                                     const Vector3& point,
                                     const AbilityDefinition* ability) {
  // Default implementation: call execute() with null target (caster-centered)
  // Subclasses override this for position-aware effects
  execute(caster, nullptr, ability);
}

Array AbilityEffect::query_units_in_sphere(const Vector3& center,
                                           float radius,
                                           Unit* exclude_unit) const {
  Array result;

  if (Engine::get_singleton()->is_editor_hint()) {
    return result;  // Don't query in editor
  }

  // For MVP, spatial queries will be implemented by subclasses
  // They have access to the scene tree and can perform their own queries
  // This helper is available for use but the base implementation returns empty
  // Future: Could implement physics server queries here if needed

  return result;
}
