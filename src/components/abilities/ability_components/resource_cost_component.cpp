#include "resource_cost_component.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "../ability_context.hpp"
#include "../ability_node.hpp"
#include "../../../core/unit.hpp"
#include "../../../debug/debug_macros.hpp"
#include "../../resources/resource_pool_component.hpp"

using godot::ClassDB;
using godot::D_METHOD;
using godot::Object;
using godot::PropertyInfo;
using godot::Ref;
using godot::RefCounted;
using godot::String;
using godot::Variant;

ResourceCostComponent::ResourceCostComponent() = default;

ResourceCostComponent::~ResourceCostComponent() = default;

void ResourceCostComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_resource_pool_id", "pool_id"),
                       &ResourceCostComponent::set_resource_pool_id);
  ClassDB::bind_method(D_METHOD("get_resource_pool_id"),
                       &ResourceCostComponent::get_resource_pool_id);
  ADD_PROPERTY(PropertyInfo(Variant::STRING, "resource_pool_id"),
               "set_resource_pool_id", "get_resource_pool_id");

  ClassDB::bind_method(D_METHOD("set_resource_cost", "cost"),
                       &ResourceCostComponent::set_resource_cost);
  ClassDB::bind_method(D_METHOD("get_resource_cost"),
                       &ResourceCostComponent::get_resource_cost);
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "resource_cost"),
               "set_resource_cost", "get_resource_cost");

  ClassDB::bind_method(D_METHOD("_on_validate", "context"),
                       &ResourceCostComponent::_on_validate);
  ClassDB::bind_method(D_METHOD("_on_activated", "context"),
                       &ResourceCostComponent::_on_activated);
}

void ResourceCostComponent::_ready() {
  AbilitySubcomponent::_ready();

  AbilityNode* ability = get_ability();
  if (ability != nullptr) {
    ability->connect("validate",
                     godot::Callable(this, "_on_validate"));
    ability->connect("activated",
                     godot::Callable(this, "_on_activated"));
  }
}

void ResourceCostComponent::set_resource_pool_id(const String& pool_id) {
  resource_pool_id = pool_id;
}

String ResourceCostComponent::get_resource_pool_id() const {
  return resource_pool_id;
}

void ResourceCostComponent::set_resource_cost(float cost) {
  resource_cost = cost;
}

float ResourceCostComponent::get_resource_cost() const {
  return resource_cost;
}

ResourcePoolComponent* ResourceCostComponent::_find_pool(
    const Ref<RefCounted>& context) {
  if (resource_cost <= 0.0f) {
    return nullptr;
  }

  Ref<AbilityContext> ctx = context;
  if (ctx.is_null()) {
    return nullptr;
  }

  Unit* caster = ctx->get_caster();
  if (caster == nullptr) {
    return nullptr;
  }

  // Find ResourcePoolComponent on the caster with matching pool_id
  for (int i = 0; i < caster->get_child_count(); i++) {
    godot::Node* child = caster->get_child(i);
    ResourcePoolComponent* pool =
        Object::cast_to<ResourcePoolComponent>(child);
    if (pool != nullptr && pool->get_pool_id() == resource_pool_id) {
      return pool;
    }
  }

  return nullptr;
}

void ResourceCostComponent::_on_validate(const Ref<RefCounted>& context) {
  if (resource_cost <= 0.0f) {
    return;  // Free ability
  }

  ResourcePoolComponent* pool = _find_pool(context);
  if (pool == nullptr) {
    Ref<AbilityContext> ctx = context;
    if (ctx.is_valid()) {
      ctx->block("no_resource_pool");
    }
    return;
  }

  if (!pool->can_spend(resource_cost)) {
    Ref<AbilityContext> ctx = context;
    if (ctx.is_valid()) {
      ctx->block("insufficient_resources");
    }
  }
}

void ResourceCostComponent::_on_activated(const Ref<RefCounted>& context) {
  if (resource_cost <= 0.0f) {
    return;
  }

  ResourcePoolComponent* pool = _find_pool(context);
  if (pool != nullptr) {
    pool->try_spend(resource_cost);
    DBG_INFO("ResourceCostComponent",
             "Spent " + String::num(resource_cost) + " from " +
                 resource_pool_id);
  }
}
