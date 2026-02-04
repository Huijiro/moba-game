# Ability system components
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./ability_types.hpp
  ./ability_node.hpp
  ./ability_node.cpp
  ./ability_api.hpp
  ./ability_api.cpp
  ./ability_effect.hpp
  ./ability_effect.cpp
  ./ability_definition.hpp
  ./ability_definition.cpp
  ./ability_component.hpp
  ./ability_component.cpp
)

# Effects subdirectory
add_subdirectory(effects)

# Ability implementations subdirectory
add_subdirectory(implementations)
