# Ability system components
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./ability_types.hpp
  ./ability_node.hpp
  ./ability_node.cpp
  ./ability_api.hpp
  ./ability_api.cpp
  ./ability_component.hpp
  ./ability_component.cpp
)

# Ability implementations subdirectory
add_subdirectory(implementations)
