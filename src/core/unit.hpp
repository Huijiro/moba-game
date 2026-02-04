#ifndef GDEXTENSION_UNIT_H
#define GDEXTENSION_UNIT_H

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/variant/string.hpp>
#include <map>
#include <vector>

namespace godot {
class StringName;
class Callable;
}  // namespace godot

using godot::Callable;
using godot::CharacterBody3D;
using godot::String;
using godot::StringName;
using godot::Variant;

class LabelRegistry;

/// Character entity with components - pure signal relay hub
/// The playable/NPC unit that receives orders from network/input/AI and relays
/// signals
///
/// Pure Signal Relay Hub
///
/// Unit is a minimal container that:
/// - Holds basic metadata (faction_id, unit_name)
/// - Provides relay() method for signal dispatch
/// - Does NOT define, track, or care about any signals
/// - Does NOT contain any game logic
///
/// Architecture:
/// - Unit is a CharacterBody3D container for components
/// - relay() is the ONLY public interface for communication
/// - Unit emits whatever signal is requested and forgets about it
/// - Components independently listen to signals via relay()
/// - No signal contracts, dependencies, or implicit assumptions
///
/// Signal Design:
/// - Signals are defined in src/common/unit_signals.hpp (as constants)
/// - Components document which signals they listen to in their headers
/// - Each component decides which signals it cares about
/// - Components that need to talk use relay() through Unit
///
/// To Use:
/// 1. Add Unit to scene
/// 2. Add desired components as children
/// 3. Components auto-wire via _ready() and connect to signals
/// 4. InputManager/AI/Network calls unit->relay(signal_name, ...)
/// 5. Unit emits, components independently listen and respond
class Unit : public CharacterBody3D {
  GDCLASS(Unit, CharacterBody3D)

 protected:
  static void _bind_methods();

 public:
  Unit();
  ~Unit();

  void _ready() override;

  // Signal relay - relays arbitrary signals to all listeners
  // Fire and forget: Unit doesn't care what signals or who listens
  // Handles dynamic signal emission without requiring pre-registration
  void relay(const StringName& signal_name);
  void relay(const StringName& signal_name, const Variant& arg1);
  void relay(const StringName& signal_name,
             const Variant& arg1,
             const Variant& arg2);
  void relay(const StringName& signal_name,
             const Variant& arg1,
             const Variant& arg2,
             const Variant& arg3);

  // Signal multiplexing - used by components to connect to dynamic signals
  void connect_signal(const StringName& signal_name, const Callable& callable);
  void disconnect_signal(const StringName& signal_name,
                         const Callable& callable);

  // Metadata
  void set_faction_id(int32_t new_faction_id);
  int32_t get_faction_id() const;

  void set_unit_name(const String& name);
  String get_unit_name() const;

  // Debug label registration - called by LabelComponent
  void register_all_debug_labels(LabelRegistry* registry);

  // TODO: Refactor ability system to use relay() instead of direct component
  // lookups Temporary methods to unblock build - these should be removed after
  // abilities are redesigned
  godot::Node* get_component_by_class(
      const godot::StringName& class_name) const;
  class AbilityComponent* get_ability_component() const;

 private:
  int32_t faction_id = 0;
  String unit_name = "Unit";

  // Signal multiplexer: maps signal names to lists of listeners (Callables)
  // This allows dynamic signal emission without pre-registering signals
  std::map<StringName, std::vector<Callable>> signal_listeners;
};

#endif  // GDEXTENSION_UNIT_H
