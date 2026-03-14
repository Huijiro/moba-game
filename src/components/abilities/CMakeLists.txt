# Ability system components
target_sources(
  ${PROJECT_NAME} PRIVATE
  ./ability_context.hpp
  ./ability_context.cpp
  ./ability_node.hpp
  ./ability_node.cpp
  ./ability_subcomponent.hpp
  ./ability_subcomponent.cpp
  ./ability_component.hpp
  ./ability_component.cpp
  ./targeting_info.hpp
  ./targeting_info.cpp
)

# Concrete ability subcomponents
add_subdirectory(ability_components)
