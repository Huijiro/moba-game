#ifndef TARGETING_PREVIEW_HPP
#define TARGETING_PREVIEW_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/templates/vector.hpp>

class AbilityNode;
class AbilitySubcomponent;
class AoEDamageComponent;
class CursorWorldQuery;
class AbilityTargetingHandler;
class Unit;

/// Orchestrates targeting preview visuals during ability targeting mode.
///
/// Each frame while targeting is active, updates preview nodes created
/// by ability subcomponents. Components provide create_preview(),
/// update_preview(), and cleanup_preview() methods.
///
/// Sibling of CursorWorldQuery and AbilityTargetingHandler under PlayerInput.
class TargetingPreview : public godot::Node {
  GDCLASS(TargetingPreview, godot::Node);

 public:
  TargetingPreview();
  ~TargetingPreview();

  void _ready() override;
  void _process(double delta) override;

  void set_controlled_unit(Unit* unit);
  Unit* get_controlled_unit() const;

 protected:
  static void _bind_methods();

 private:
  CursorWorldQuery* cursor_query = nullptr;
  AbilityTargetingHandler* targeting_handler = nullptr;
  Unit* controlled_unit = nullptr;

  // Track whether we're currently showing previews
  bool previews_active = false;
  int current_slot = -1;

  // Active preview data
  struct PreviewEntry {
    AbilitySubcomponent* component = nullptr;     // for AbilitySubcomponent
    AoEDamageComponent* aoe_component = nullptr;  // for AoEDamageComponent
    godot::Node3D* preview_node = nullptr;
  };
  godot::Vector<PreviewEntry> active_previews;

  void _activate_previews(int slot);
  void _update_previews();
  void _deactivate_previews();

  AbilityNode* _find_ability(int slot) const;
};

#endif  // TARGETING_PREVIEW_HPP
