#ifndef VFX_COMPONENT_HPP
#define VFX_COMPONENT_HPP

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#include "../ability_subcomponent.hpp"

class Unit;

/// Triggers VFX playback on a lifecycle signal.
///
/// Finds a named VFX template child of the parent AbilityNode,
/// hides it (it's a template), and duplicates + plays it when triggered.
///
/// Properties:
///   vfx_name       — name of the VFX child node to duplicate
///   trigger_signal — which ability lifecycle signal triggers playback
///                    ("execute", "activated", "completed", "tick")
///   position_mode  — where to spawn: AT_CASTER (0), AT_TARGET (1), AT_POSITION (2)
class VFXComponent : public AbilitySubcomponent {
  GDCLASS(VFXComponent, AbilitySubcomponent);

 public:
  enum PositionMode {
    AT_CASTER = 0,
    AT_TARGET = 1,
    AT_POSITION = 2,
  };

  VFXComponent();
  ~VFXComponent();

  void _ready() override;

  void set_trigger_signal(const godot::String& signal);
  godot::String get_trigger_signal() const;

  void set_position_mode(int mode);
  int get_position_mode() const;

  void set_vfx_signal(const godot::String& signal);
  godot::String get_vfx_signal() const;

  void set_emit_execute_on_vfx_signal(bool value);
  bool get_emit_execute_on_vfx_signal() const;

 protected:
  static void _bind_methods();

 private:
  godot::String trigger_signal = "execute";
  int position_mode = AT_CASTER;

  /// Name of the VFX animation signal to listen for (e.g. "explosion_damage").
  /// When the VFX emits this signal, and emit_execute_on_vfx_signal is true,
  /// VFXComponent emits "execute" on the parent AbilityNode.
  godot::String vfx_signal = "";
  bool emit_execute_on_vfx_signal = false;

  /// Stored context from trigger, used when re-emitting execute.
  godot::Ref<godot::RefCounted> last_context;

  void _on_triggered(const godot::Ref<godot::RefCounted>& context);
  void _on_vfx_signal();
};

VARIANT_ENUM_CAST(VFXComponent::PositionMode);

#endif  // VFX_COMPONENT_HPP
