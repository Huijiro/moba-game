# Ability system components
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./ability_types.hpp
  ./ability_context.hpp
  ./ability_context.cpp
  ./ability_node.hpp
  ./ability_node.cpp
  ./ability_subcomponent.hpp
  ./ability_subcomponent.cpp
  ./ability_api.hpp
  ./ability_api.cpp
  ./ability_component.hpp
  ./ability_component.cpp
  ./targeting_info.hpp
  ./targeting_info.cpp
  ./modules/ability_module.hpp
  ./modules/ability_module.cpp
  ./modules/ability_targeting_module.hpp
  ./modules/ability_targeting_module.cpp
  ./modules/ability_cast_module.hpp
  ./modules/ability_cast_module.cpp
  ./modules/ability_range_module.hpp
  ./modules/ability_range_module.cpp
  ./modules/ability_effect_module.hpp
  ./modules/ability_effect_module.cpp
)

# Ability subcomponent implementations subdirectory
add_subdirectory(ability_components)

# Legacy ability implementations subdirectory
add_subdirectory(implementations)
