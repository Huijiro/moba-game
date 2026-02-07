#include "vfx_manager.hpp"

#include <algorithm>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../components/abilities/ability_node.hpp"
#include "../core/unit.hpp"
#include "../debug/debug_macros.hpp"
#include "vfx_node.hpp"

using godot::ClassDB;
using godot::Object;
using godot::ResourceLoader;
using godot::UtilityFunctions;

VFXManager* VFXManager::singleton_instance = nullptr;

VFXManager::VFXManager() {
  DBG_INFO("VFXManager", "Initializing VFX Manager singleton");
}

VFXManager::~VFXManager() {
  DBG_INFO("VFXManager", "Destroying VFX Manager singleton");
  // Clean up all pooled VFX
  ability_vfx_pools.clear();
}

VFXManager* VFXManager::get_instance() {
  if (singleton_instance == nullptr) {
    singleton_instance = new VFXManager();
  }
  return singleton_instance;
}

void VFXManager::initialize(Unit* unit) {
  if (unit == nullptr) {
    DBG_ERROR("VFXManager", "Cannot initialize with null unit");
    return;
  }

  DBG_INFO("VFXManager", "Initializing VFX for unit: " + unit->get_name());

  // Scan all components in the unit for AbilityComponent
  for (int i = 0; i < unit->get_child_count(); i++) {
    Node* child = unit->get_child(i);
    if (child == nullptr)
      continue;

    // Check if this is an AbilityComponent
    if (child->is_class("AbilityComponent")) {
      // Get the ability component and register its abilities
      // For now, we'll iterate through abilities directly from the scene
      // In a full implementation, AbilityComponent would expose abilities
    }
  }
}

void VFXManager::register_ability_vfx(AbilityNode* ability) {
  if (ability == nullptr) {
    DBG_ERROR("VFXManager", "Cannot register null ability");
    return;
  }

  DBG_INFO("VFXManager", "Registering VFX for ability: " + ability->get_name());

  _extract_vfx_from_ability(ability);
}

bool VFXManager::play_vfx(AbilityNode* ability,
                          const String& vfx_name,
                          const Dictionary& params) {
  if (ability == nullptr) {
    DBG_ERROR("VFXManager", "Cannot play VFX with null ability");
    return false;
  }

  VFXNode* vfx = _get_pooled_vfx(ability, vfx_name);
  if (vfx == nullptr) {
    DBG_WARN("VFXManager",
             "No VFX found: " + ability->get_name() + "." + vfx_name);
    return false;
  }

  DBG_INFO("VFXManager",
           "Playing VFX: " + ability->get_name() + "." + vfx_name);

  // Play the VFX with parameters
  vfx->play(params);

  return true;
}

VFXNode* VFXManager::_get_pooled_vfx(AbilityNode* ability,
                                     const String& vfx_name) {
  if (ability == nullptr) {
    return nullptr;
  }

  // Get or create pool for this ability
  void* ability_key = (void*)ability;
  auto& ability_pools = ability_vfx_pools[ability_key];

  // Get pool for this VFX name
  VFXPool& pool = ability_pools[vfx_name];

  // Try to get an available VFX from pool
  if (!pool.available.empty()) {
    VFXNode* vfx = pool.available.back();
    pool.available.pop_back();
    pool.in_use.push_back(vfx);

    // Reset VFX state
    if (vfx->is_inside_tree()) {
      vfx->show();  // Make visible
    }

    return vfx;
  }

  // Pool is empty - no VFX available
  return nullptr;
}

void VFXManager::_return_to_pool(AbilityNode* ability,
                                 const String& vfx_name,
                                 VFXNode* vfx) {
  if (ability == nullptr || vfx == nullptr) {
    return;
  }

  void* ability_key = (void*)ability;
  auto it = ability_vfx_pools.find(ability_key);
  if (it == ability_vfx_pools.end()) {
    return;
  }

  auto& ability_pools = it->second;
  auto pool_it = ability_pools.find(vfx_name);
  if (pool_it == ability_pools.end()) {
    return;
  }

  VFXPool& pool = pool_it->second;

  // Find and remove from in_use
  auto in_use_it = std::find(pool.in_use.begin(), pool.in_use.end(), vfx);
  if (in_use_it != pool.in_use.end()) {
    pool.in_use.erase(in_use_it);
  }

  // Add back to available
  pool.available.push_back(vfx);

  // Hide until needed again
  if (vfx->is_inside_tree()) {
    vfx->hide();
  }
}

void VFXManager::_extract_vfx_from_ability(AbilityNode* ability) {
  if (ability == nullptr) {
    return;
  }

  // Get the ability's effect root (the ability node itself or a child
  // container)
  Node* effect_root = ability;

  // Scan children for VFXNode instances
  for (int i = 0; i < effect_root->get_child_count(); i++) {
    Node* child = effect_root->get_child(i);
    if (child == nullptr)
      continue;

    // Check if child is a VFXNode
    VFXNode* vfx = Object::cast_to<VFXNode>(child);
    if (vfx != nullptr) {
      String vfx_name = child->get_name();
      DBG_INFO("VFXManager",
               "Found VFX: " + ability->get_name() + "." + vfx_name);

      // Add to available pool
      void* ability_key = (void*)ability;
      ability_vfx_pools[ability_key][vfx_name].available.push_back(vfx);
    }
  }
}

Ref<PackedScene> VFXManager::_preload_scene(const String& path) {
  // Check if already cached
  auto it = preloaded_scenes.find(path);
  if (it != preloaded_scenes.end()) {
    return it->second;
  }

  // Load the scene
  Ref<PackedScene> scene = ResourceLoader::get_singleton()->load(path);
  if (scene.is_null()) {
    DBG_WARN("VFXManager", "Failed to load scene: " + path);
    return nullptr;
  }

  preloaded_scenes[path] = scene;
  return scene;
}
