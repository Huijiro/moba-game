target_sources(
  ${PROJECT_NAME} PRIVATE
  
  # Root level files
  ./register_types.hpp
  ./register_types.cpp
  ./unit_order.hpp
  
  # Core systems
  ./core/unit/unit.hpp
  ./core/unit/unit.cpp
  
  ./core/input_manager/input_manager.hpp
  ./core/input_manager/input_manager.cpp
  
  ./core/moba_camera/moba_camera.hpp
  ./core/moba_camera/moba_camera.cpp
  
  ./core/match_manager/match_manager.hpp
  ./core/match_manager/match_manager.cpp
  
  # Base component class
  ./components/unit_component/unit_component.hpp
  ./components/unit_component/unit_component.cpp
  
  # Gameplay components
  ./components/movement_component/movement_component.hpp
  ./components/movement_component/movement_component.cpp
  
  ./components/health_component/health_component.hpp
  ./components/health_component/health_component.cpp
  
  ./components/attack_component/attack_component.hpp
  ./components/attack_component/attack_component.cpp
  ./components/attack_component/projectile.hpp
  ./components/attack_component/projectile.cpp
  
  ./components/animation_controller/animation_controller.hpp
  ./components/animation_controller/animation_controller.cpp
  
  ./components/resource_pool_component/resource_pool_component.hpp
  ./components/resource_pool_component/resource_pool_component.cpp
  
  # Test/Debug components
  ./components/test_movement/test_movement.hpp
  ./components/test_movement/test_movement.cpp
  
  # World systems
  ./world/interactable.hpp
  ./world/interactable.cpp
  
  # Utilities
  ./utils/beeper.h
  ./utils/beeper.cpp
)

# Include directories for all subdirectories
# This allows headers to be included by name from any subdirectory
target_include_directories(${PROJECT_NAME} PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/core/unit"
  "${CMAKE_CURRENT_SOURCE_DIR}/core/input_manager"
  "${CMAKE_CURRENT_SOURCE_DIR}/core/moba_camera"
  "${CMAKE_CURRENT_SOURCE_DIR}/core/match_manager"
  "${CMAKE_CURRENT_SOURCE_DIR}/components/unit_component"
  "${CMAKE_CURRENT_SOURCE_DIR}/components/movement_component"
  "${CMAKE_CURRENT_SOURCE_DIR}/components/health_component"
  "${CMAKE_CURRENT_SOURCE_DIR}/components/attack_component"
  "${CMAKE_CURRENT_SOURCE_DIR}/components/animation_controller"
  "${CMAKE_CURRENT_SOURCE_DIR}/components/resource_pool_component"
  "${CMAKE_CURRENT_SOURCE_DIR}/components/test_movement"
  "${CMAKE_CURRENT_SOURCE_DIR}/world"
  "${CMAKE_CURRENT_SOURCE_DIR}/utils"
)
