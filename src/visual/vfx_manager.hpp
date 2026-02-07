#ifndef GDEXTENSION_VFX_MANAGER_H
#define GDEXTENSION_VFX_MANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>
#include <map>
#include <vector>

using godot::Dictionary;
using godot::Node;
using godot::PackedScene;
using godot::Ref;
using godot::String;

class Unit;
class AbilityNode;
class VFXNode;

/// Singleton manager for VFX pooling and triggering
/// Responsible for:
/// 1. Preloading ability scene files
/// 2. Extracting and pooling VFX nodes
/// 3. Triggering VFX by ability + name with parameters
///
/// Usage:
/// - Call initialize(unit) when unit with abilities is created
/// - Call play_vfx(ability, vfx_name, params) to trigger effects
/// - Manager handles pooling/cleanup automatically
class VFXManager {
 public:
  /// Get singleton instance
  static VFXManager* get_instance();

  /// Initialize manager when a unit with abilities is added
  /// Scans ability scenes and sets up VFX pools
  void initialize(Unit* unit);

  /// Register an ability's VFX for pooling
  /// Called automatically during initialize()
  void register_ability_vfx(AbilityNode* ability);

  /// Trigger a VFX by ability and name with custom parameters
  /// Returns true if VFX was found and played
  bool play_vfx(AbilityNode* ability,
                const String& vfx_name,
                const Dictionary& params = {});

 private:
  VFXManager();
  ~VFXManager();

  // Pooling infrastructure
  struct VFXPool {
    std::vector<VFXNode*> available;  // Ready to use
    std::vector<VFXNode*> in_use;     // Currently playing
  };

  // Map from ability pointer + vfx_name to pool
  std::map<void*, std::map<String, VFXPool>> ability_vfx_pools;

  // Map from scene path to preloaded scene
  std::map<String, Ref<PackedScene>> preloaded_scenes;

  /// Get or create a pooled VFX instance for an ability
  VFXNode* _get_pooled_vfx(AbilityNode* ability, const String& vfx_name);

  /// Return a VFX to the pool after use
  void _return_to_pool(AbilityNode* ability,
                       const String& vfx_name,
                       VFXNode* vfx);

  /// Extract all VFX children from an ability scene
  void _extract_vfx_from_ability(AbilityNode* ability);

  /// Preload a scene file
  Ref<PackedScene> _preload_scene(const String& path);

  static VFXManager* singleton_instance;
};

#endif  // GDEXTENSION_VFX_MANAGER_H
