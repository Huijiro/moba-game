#include "vfx_component.hpp"

#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Dictionary;
using godot::Node3D;
using godot::Object;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::String;
using godot::Variant;
using godot::Vector3;

VFXComponent::VFXComponent() = default;

VFXComponent::~VFXComponent() = default;

void VFXComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_trigger_signal", "signal"),
                       &VFXComponent::set_trigger_signal);
  ClassDB::bind_method(D_METHOD("get_trigger_signal"),
                       &VFXComponent::get_trigger_signal);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "trigger_signal"),
               "set_trigger_signal", "get_trigger_signal");

  ClassDB::bind_method(D_METHOD("set_position_mode", "mode"),
                       &VFXComponent::set_position_mode);
  ClassDB::bind_method(D_METHOD("get_position_mode"),
                       &VFXComponent::get_position_mode);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "position_mode",
                            godot::PROPERTY_HINT_ENUM,
                            "At Caster,At Target,At Position"),
               "set_position_mode", "get_position_mode");

  ClassDB::bind_method(D_METHOD("set_vfx_signal", "signal"),
                       &VFXComponent::set_vfx_signal);
  ClassDB::bind_method(D_METHOD("get_vfx_signal"),
                       &VFXComponent::get_vfx_signal);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "vfx_signal"),
               "set_vfx_signal", "get_vfx_signal");

  ClassDB::bind_method(D_METHOD("set_emit_execute_on_vfx_signal", "value"),
                       &VFXComponent::set_emit_execute_on_vfx_signal);
  ClassDB::bind_method(D_METHOD("get_emit_execute_on_vfx_signal"),
                       &VFXComponent::get_emit_execute_on_vfx_signal);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "emit_execute_on_vfx_signal"),
               "set_emit_execute_on_vfx_signal",
               "get_emit_execute_on_vfx_signal");

  ClassDB::bind_method(D_METHOD("_on_triggered", "context"),
                       &VFXComponent::_on_triggered);
  ClassDB::bind_method(D_METHOD("_on_vfx_signal"),
                       &VFXComponent::_on_vfx_signal);

  BIND_ENUM_CONSTANT(AT_CASTER);
  BIND_ENUM_CONSTANT(AT_TARGET);
  BIND_ENUM_CONSTANT(AT_POSITION);
}

void VFXComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability == nullptr) {
    return;
  }

  // Hide VFX template children — they're just templates for duplication
  for (int i = 0; i < get_child_count(); i++) {
    Node3D* child = Object::cast_to<Node3D>(get_child(i));
    if (child != nullptr) {
      child->set_visible(false);
    }
  }

  // Connect to the trigger signal
  if (!trigger_signal.is_empty() && ability->has_signal(trigger_signal)) {
    ability->connect(trigger_signal,
                     godot::Callable(this, "_on_triggered"));
    DBG_DEBUG("VFXComponent",
              "Connected to '" + trigger_signal + "' for VFX");
  } else {
    DBG_WARN("VFXComponent",
             "Signal '" + trigger_signal + "' not found on " +
                 ability->get_ability_name());
  }
}

void VFXComponent::_on_triggered(const Ref<RefCounted>& context) {
  AbilityNode* ability = get_ability();
  if (ability == nullptr) {
    return;
  }

  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) {
    return;
  }

  Unit* caster = ctx->get_caster();
  if (caster == nullptr || !caster->is_inside_tree()) {
    return;
  }

  // Determine position based on mode
  Vector3 pos;
  switch (position_mode) {
    case AT_CASTER:
      pos = caster->get_global_position();
      break;
    case AT_TARGET: {
      Unit* target = ctx->get_target();
      if (target != nullptr && target->is_inside_tree()) {
        pos = target->get_global_position();
      } else {
        pos = caster->get_global_position();
      }
      break;
    }
    case AT_POSITION:
      pos = ctx->get_position();
      if (pos == Vector3()) {
        pos = caster->get_global_position();
      }
      break;
  }

  // Store context for VFX signal forwarding
  last_context = context;

  // Find and duplicate the VFX template (first Node3D child)
  godot::Node* template_node = nullptr;
  for (int i = 0; i < get_child_count(); i++) {
    Node3D* child = Object::cast_to<Node3D>(get_child(i));
    if (child != nullptr) {
      template_node = child;
      break;
    }
  }

  if (template_node == nullptr) {
    DBG_WARN("VFXComponent", "No VFX template child found");
    return;
  }

  godot::Node* vfx_instance = template_node->duplicate();
  if (vfx_instance == nullptr) {
    return;
  }

  // Add to world (caster's parent, so it's in the scene root)
  godot::Node* world = caster->get_parent();
  if (world == nullptr) {
    vfx_instance->queue_free();
    return;
  }

  world->add_child(vfx_instance);

  // Position the VFX (scale comes from the template scene itself)
  Node3D* vfx_3d = Object::cast_to<Node3D>(vfx_instance);
  if (vfx_3d != nullptr) {
    vfx_3d->set_visible(true);
    vfx_3d->set_global_position(pos);
  }

  // Start any AnimationPlayer on the spawned instance
  if (vfx_instance != nullptr) {
    for (int i = 0; i < vfx_instance->get_child_count(); i++) {
      godot::AnimationPlayer* ap =
          Object::cast_to<godot::AnimationPlayer>(vfx_instance->get_child(i));
      if (ap != nullptr) {
        // Play the first animation in the first library
        godot::TypedArray<godot::StringName> libs = ap->get_animation_library_list();
        for (int l = 0; l < libs.size(); l++) {
          godot::Ref<godot::AnimationLibrary> lib =
              ap->get_animation_library(libs[l]);
          if (lib.is_valid()) {
            godot::TypedArray<godot::StringName> anims =
                lib->get_animation_list();
            for (int a = 0; a < anims.size(); a++) {
              godot::String anim_name = anims[a];
              if (anim_name != "RESET") {
                ap->play(anim_name);
                break;
              }
            }
          }
          break;
        }
        break;
      }
    }
  }

  // If we need to listen for a VFX animation signal, connect to the spawned instance
  if (emit_execute_on_vfx_signal && !vfx_signal.is_empty()) {
    if (!vfx_instance->has_signal(vfx_signal)) {
      vfx_instance->add_user_signal(vfx_signal);
    }
    vfx_instance->connect(vfx_signal,
                          godot::Callable(this, "_on_vfx_signal"));
    DBG_DEBUG("VFXComponent",
              "Listening for VFX signal '" + vfx_signal + "' to emit execute");
  }

  DBG_DEBUG("VFXComponent",
            ability->get_ability_name() + " spawned VFX at (" +
                String::num(pos.x, 1) + ", " + String::num(pos.z, 1) + ")");
}

void VFXComponent::_on_vfx_signal() {
  AbilityNode* ability = get_ability();
  if (ability == nullptr || last_context.is_null()) {
    return;
  }

  DBG_INFO("VFXComponent",
           ability->get_ability_name() + " VFX signal '" + vfx_signal +
               "' → emitting execute");
  ability->emit_signal("execute", last_context);
}

void VFXComponent::set_trigger_signal(const String& signal) {
  trigger_signal = signal;
}
String VFXComponent::get_trigger_signal() const { return trigger_signal; }

void VFXComponent::set_position_mode(int mode) { position_mode = mode; }
int VFXComponent::get_position_mode() const { return position_mode; }

void VFXComponent::set_vfx_signal(const String& signal) { vfx_signal = signal; }
String VFXComponent::get_vfx_signal() const { return vfx_signal; }

void VFXComponent::set_emit_execute_on_vfx_signal(bool value) {
  emit_execute_on_vfx_signal = value;
}
bool VFXComponent::get_emit_execute_on_vfx_signal() const {
  return emit_execute_on_vfx_signal;
}
