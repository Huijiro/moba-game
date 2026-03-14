#ifndef GDEXTENSION_ABILITY_CONTEXT_H
#define GDEXTENSION_ABILITY_CONTEXT_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

class AbilityNode;
class Unit;

/// Shared runtime context passed through ability lifecycle signals.
///
/// RefCounted so it can flow through Godot signals as Ref<AbilityContext>.
/// Components read caster/target/position and can block the cast via block().
class AbilityContext : public godot::RefCounted {
  GDCLASS(AbilityContext, godot::RefCounted);

 public:
  AbilityContext();
  ~AbilityContext();

  // Caster and target
  void set_caster(Unit* unit);
  Unit* get_caster() const;

  void set_target(Unit* unit);
  Unit* get_target() const;

  void set_position(const godot::Vector3& pos);
  godot::Vector3 get_position() const;

  void set_slot(int s);
  int get_slot() const;

  // Validation
  void block(const godot::String& reason);
  bool is_blocked() const;
  godot::String get_block_reason() const;

  // Reset for reuse
  void reset();

 protected:
  static void _bind_methods();

 private:
  Unit* caster = nullptr;
  Unit* target = nullptr;
  godot::Vector3 position = godot::Vector3();
  int slot = -1;
  bool blocked = false;
  godot::String block_reason = "";
};

#endif  // GDEXTENSION_ABILITY_CONTEXT_H
