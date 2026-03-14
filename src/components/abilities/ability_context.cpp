#include "ability_context.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../../core/unit.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::PropertyInfo;
using godot::String;
using godot::Variant;
using godot::Vector3;

AbilityContext::AbilityContext() = default;

AbilityContext::~AbilityContext() = default;

void AbilityContext::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_caster", "unit"),
                       &AbilityContext::set_caster);
  ClassDB::bind_method(D_METHOD("get_caster"),
                       &AbilityContext::get_caster);

  ClassDB::bind_method(D_METHOD("set_target", "unit"),
                       &AbilityContext::set_target);
  ClassDB::bind_method(D_METHOD("get_target"),
                       &AbilityContext::get_target);

  ClassDB::bind_method(D_METHOD("set_position", "pos"),
                       &AbilityContext::set_position);
  ClassDB::bind_method(D_METHOD("get_position"),
                       &AbilityContext::get_position);

  ClassDB::bind_method(D_METHOD("set_slot", "slot"),
                       &AbilityContext::set_slot);
  ClassDB::bind_method(D_METHOD("get_slot"),
                       &AbilityContext::get_slot);

  ClassDB::bind_method(D_METHOD("block", "reason"),
                       &AbilityContext::block);
  ClassDB::bind_method(D_METHOD("is_blocked"),
                       &AbilityContext::is_blocked);
  ClassDB::bind_method(D_METHOD("get_block_reason"),
                       &AbilityContext::get_block_reason);

  ClassDB::bind_method(D_METHOD("reset"),
                       &AbilityContext::reset);
}

void AbilityContext::set_caster(Unit* unit) {
  caster = unit;
}

Unit* AbilityContext::get_caster() const {
  return caster;
}

void AbilityContext::set_target(Unit* unit) {
  target = unit;
}

Unit* AbilityContext::get_target() const {
  return target;
}

void AbilityContext::set_position(const Vector3& pos) {
  position = pos;
}

Vector3 AbilityContext::get_position() const {
  return position;
}

void AbilityContext::set_slot(int s) {
  slot = s;
}

int AbilityContext::get_slot() const {
  return slot;
}

void AbilityContext::block(const String& reason) {
  blocked = true;
  block_reason = reason;
}

bool AbilityContext::is_blocked() const {
  return blocked;
}

String AbilityContext::get_block_reason() const {
  return block_reason;
}

void AbilityContext::reset() {
  caster = nullptr;
  target = nullptr;
  position = Vector3();
  slot = -1;
  blocked = false;
  block_reason = "";
}
