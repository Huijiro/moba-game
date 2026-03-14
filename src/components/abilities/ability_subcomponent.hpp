#ifndef GDEXTENSION_ABILITY_SUBCOMPONENT_H
#define GDEXTENSION_ABILITY_SUBCOMPONENT_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

using godot::Node;

class AbilityNode;
class LabelRegistry;

/// Base class for ability child components.
/// Mirrors UnitComponent: validates parentage and exposes get_ability().
class AbilitySubcomponent : public Node {
  GDCLASS(AbilitySubcomponent, Node)

 protected:
  static void _bind_methods();

  AbilityNode* owner_ability = nullptr;

 public:
  AbilitySubcomponent();
  ~AbilitySubcomponent();

  void _ready() override;

  AbilityNode* get_ability() const;

  /// Optional: Register debug label data. Override in subclasses to expose
  /// debug info.
  virtual void register_debug_labels(LabelRegistry* registry) {
    // Default: do nothing
  }

  // --- Targeting preview interface ---
  // Override in subcomponents that provide targeting previews.
  // TargetingPreview node calls these when the player enters targeting mode.

  /// Does this component provide a targeting preview?
  virtual bool has_preview() const { return false; }

  /// Create and return the preview node. Caller adds it to the scene tree.
  virtual godot::Node3D* create_preview() { return nullptr; }

  /// Update the preview each frame with current cursor/caster state.
  virtual void update_preview(godot::Node3D* preview,
                              const godot::Vector3& caster_pos,
                              const godot::Vector3& ground_pos) {}

  /// Cleanup when targeting ends. Default: queue_free the preview node.
  virtual void cleanup_preview(godot::Node3D* preview);
};

#endif  // GDEXTENSION_ABILITY_SUBCOMPONENT_H
