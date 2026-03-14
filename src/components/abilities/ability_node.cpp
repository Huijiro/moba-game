#include "ability_node.hpp"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "ability_context.hpp"
#include "ability_subcomponent.hpp"
#include "../../core/unit.hpp"
#include "../../debug/debug_macros.hpp"

using godot::Array;
using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::PropertyInfo;
using godot::UtilityFunctions;
using godot::Variant;

AbilityNode::AbilityNode() = default;

AbilityNode::~AbilityNode() = default;

void AbilityNode::_bind_methods() {
  // ========== METADATA ==========
  ADD_GROUP("Basic Info", "");

  ClassDB::bind_method(D_METHOD("set_ability_name", "name"),
                       &AbilityNode::set_ability_name);
  ClassDB::bind_method(D_METHOD("get_ability_name"),
                       &AbilityNode::get_ability_name);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "ability_name"),
               "set_ability_name", "get_ability_name");

  ClassDB::bind_method(D_METHOD("set_description", "description"),
                       &AbilityNode::set_description);
  ClassDB::bind_method(D_METHOD("get_description"),
                       &AbilityNode::get_description);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"),
               "set_description", "get_description");

  ClassDB::bind_method(D_METHOD("set_icon", "icon"), &AbilityNode::set_icon);
  ClassDB::bind_method(D_METHOD("get_icon"), &AbilityNode::get_icon);
  ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "icon",
                            godot::PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"),
               "set_icon", "get_icon");

  // ========== LIFECYCLE ==========
  ClassDB::bind_method(D_METHOD("can_cast", "context"),
                       &AbilityNode::can_cast);
  ClassDB::bind_method(D_METHOD("start_cast", "context"),
                       &AbilityNode::start_cast);

  // ========== SUBCOMPONENT DISCOVERY ==========
  ClassDB::bind_method(D_METHOD("get_ability_subcomponents"),
                       &AbilityNode::get_ability_subcomponents);

  // ========== VFX ==========
  ClassDB::bind_method(D_METHOD("play_vfx", "caster", "vfx_name", "params"),
                       &AbilityNode::play_vfx, DEFVAL(godot::Dictionary()));

  // ========== LIFECYCLE SIGNALS ==========
  ADD_SIGNAL(godot::MethodInfo("validate",
      PropertyInfo(Variant::OBJECT, "context")));
  ADD_SIGNAL(godot::MethodInfo("activated",
      PropertyInfo(Variant::OBJECT, "context")));
  ADD_SIGNAL(godot::MethodInfo("execute",
      PropertyInfo(Variant::OBJECT, "context")));
  ADD_SIGNAL(godot::MethodInfo("completed",
      PropertyInfo(Variant::OBJECT, "context")));
  ADD_SIGNAL(godot::MethodInfo("tick",
      PropertyInfo(Variant::OBJECT, "context")));
  ADD_SIGNAL(godot::MethodInfo("cooldown_started",
      PropertyInfo(Variant::OBJECT, "context")));
  ADD_SIGNAL(godot::MethodInfo("cooldown_finished",
      PropertyInfo(Variant::OBJECT, "context")));

  // ========== TARGETING QUERY ==========
  ADD_SIGNAL(godot::MethodInfo("query_targeting",
      PropertyInfo(Variant::OBJECT, "info")));
  ClassDB::bind_method(D_METHOD("get_targeting_info"),
                       &AbilityNode::get_targeting_info);
}

// ========== METADATA ACCESSORS ==========

void AbilityNode::set_ability_name(const String& name) {
  ability_name = name;
}

String AbilityNode::get_ability_name() const {
  return ability_name;
}

void AbilityNode::set_description(const String& desc) {
  description = desc;
}

String AbilityNode::get_description() const {
  return description;
}

void AbilityNode::set_icon(const Ref<Texture2D>& texture) {
  icon = texture;
}

Ref<Texture2D> AbilityNode::get_icon() const {
  return icon;
}

// ========== LIFECYCLE ==========

bool AbilityNode::can_cast(const Ref<AbilityContext>& context) {
  // Emit validate — components block via context->block("reason")
  // Godot signals are synchronous, so all handlers run before we return.
  emit_signal("validate", context);
  return !context->is_blocked();
}

void AbilityNode::start_cast(const Ref<AbilityContext>& context) {
  emit_signal("activated", context);
}

Ref<TargetingInfo> AbilityNode::get_targeting_info() {
  Ref<TargetingInfo> info;
  info.instantiate();
  emit_signal("query_targeting", info);
  return info;
}

// ========== SUBCOMPONENT DISCOVERY ==========

Array AbilityNode::get_ability_subcomponents() const {
  Array subcomponents;

  for (int i = 0; i < get_child_count(); i++) {
    Node* child = get_child(i);
    AbilitySubcomponent* subcomp =
        Object::cast_to<AbilitySubcomponent>(child);
    if (subcomp != nullptr) {
      subcomponents.append(subcomp);
    }
  }

  return subcomponents;
}

// ========== VFX SYSTEM ==========

godot::Node* AbilityNode::play_vfx(Unit* caster,
                                   const String& vfx_name,
                                   const godot::Dictionary& params) {
  DBG_INFO("AbilityNode",
           "play_vfx called for: " + ability_name + "." + vfx_name);

  if (caster == nullptr || !caster->is_inside_tree()) {
    DBG_WARN("AbilityNode", "No valid caster in tree to spawn VFX: " +
                                ability_name + "." + vfx_name);
    return nullptr;
  }

  Node* vfx_template = find_child(vfx_name, true, false);
  if (vfx_template == nullptr) {
    DBG_WARN("AbilityNode", "VFX not found: " + ability_name + "." + vfx_name);
    return nullptr;
  }

  Node* vfx_instance = vfx_template->duplicate(false);
  if (vfx_instance == nullptr) {
    DBG_WARN("AbilityNode",
             "Failed to duplicate VFX: " + ability_name + "." + vfx_name);
    return nullptr;
  }

  Node* vfx_parent = caster->get_parent();
  if (vfx_parent == nullptr) {
    DBG_WARN("AbilityNode", "Caster has no parent, cannot spawn VFX: " +
                                ability_name + "." + vfx_name);
    vfx_instance->queue_free();
    return nullptr;
  }

  vfx_parent->add_child(vfx_instance);

  auto vfx = Object::cast_to<godot::Node3D>(vfx_instance);
  if (vfx == nullptr) {
    DBG_WARN("AbilityNode",
             "VFX instance is not Node3D: " + ability_name + "." + vfx_name);
    vfx_instance->queue_free();
    return nullptr;
  }

  vfx->call("play", params);
  DBG_INFO("AbilityNode", "Triggered VFX: " + ability_name + "." + vfx_name);
  return vfx_instance;
}

void AbilityNode::_register_vfx() {
  // Deferred VFX discovery
}
