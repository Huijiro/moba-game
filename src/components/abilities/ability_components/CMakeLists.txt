# Concrete ability subcomponent implementations
target_sources(
  ${PROJECT_NAME} PRIVATE
  # Cast type components
  ./instant_cast_component.hpp
  ./instant_cast_component.cpp
  ./cast_time_component.hpp
  ./cast_time_component.cpp
  ./channel_component.hpp
  ./channel_component.cpp
  # Targeting components
  ./unit_targeting_component.hpp
  ./unit_targeting_component.cpp
  ./point_targeting_component.hpp
  ./point_targeting_component.cpp
  # Effect components
  ./damage_effect_component.hpp
  ./damage_effect_component.cpp
  ./aoe_damage_component.hpp
  ./aoe_damage_component.cpp
  ./homing_projectile_component.hpp
  ./homing_projectile_component.cpp
  ./skillshot_projectile_component.hpp
  ./skillshot_projectile_component.cpp
  # Resource components
  ./cooldown_component.hpp
  ./cooldown_component.cpp
  ./resource_cost_component.hpp
  ./resource_cost_component.cpp
  # VFX components
  ./vfx_component.hpp
  ./vfx_component.cpp
)
