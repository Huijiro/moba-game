# Ability system components
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./ability_types.hpp
  ./ability_effect.hpp
  ./ability_effect.cpp
  ./ability_definition.hpp
  ./ability_definition.cpp
  ./ability_component.hpp
  ./ability_component.cpp
)

# Effects subdirectory
add_subdirectory(effects)
