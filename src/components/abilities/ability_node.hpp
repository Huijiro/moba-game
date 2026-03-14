#ifndef ABILITY_NODE_HPP
#define ABILITY_NODE_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#include "targeting_info.hpp"

using godot::Node;
using godot::Ref;
using godot::String;
using godot::Texture2D;

class AbilityContext;
class Unit;

/// AbilityNode — thin event bus + metadata.
///
/// Defines lifecycle signals that child AbilitySubcomponents connect to.
/// Does NOT know about any specific component type.
///
/// Signals (all pass Ref<AbilityContext>):
///   validate(context)       — validation phase, components can block
///   activated(context)        — cast begins
///   execute(context)  — the "fire" moment
///   completed(context)       — cast complete
///   tick(context)        — periodic tick during channel
///   cooldown_started(context)    — cooldown begins
///   cooldown_finished(context)   — cooldown ends
///
/// Scene tree:
///   AbilityNode
///   ├── SomeTargetingComponent
///   ├── SomeCastComponent
///   ├── SomeEffectComponent
///   └── ...
class AbilityNode : public Node {
  GDCLASS(AbilityNode, Node);

 public:
  AbilityNode();
  ~AbilityNode();

  // ========== METADATA ==========
  void set_ability_name(const String& name);
  String get_ability_name() const;

  void set_description(const String& desc);
  String get_description() const;

  void set_icon(const Ref<Texture2D>& texture);
  Ref<Texture2D> get_icon() const;

  // ========== LIFECYCLE ==========
  // Validation: emits validate, returns true if no component blocked.
  bool can_cast(const Ref<AbilityContext>& context);

  // Begins the cast: emits activated.
  void start_cast(const Ref<AbilityContext>& context);

  // Query targeting: emits query_targeting, targeting components fill in info.
  Ref<TargetingInfo> get_targeting_info();

  // ========== SUBCOMPONENT DISCOVERY ==========
  godot::Array get_ability_subcomponents() const;

  // ========== VFX ==========
  godot::Node* play_vfx(Unit* caster,
                        const String& vfx_name,
                        const godot::Dictionary& params = godot::Dictionary());

 protected:
  static void _bind_methods();

 private:
  // ========== METADATA ONLY ==========
  String ability_name = "Ability";
  String description = "";
  Ref<Texture2D> icon = nullptr;

  void _register_vfx();
};

#endif  // ABILITY_NODE_HPP
