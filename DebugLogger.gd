extends Node

## Autoload script for initializing and registering the custom DebugLogger
##
## SETUP INSTRUCTIONS:
## 1. In Godot Editor: Project Settings > Autoload
## 2. Add this script with:
##    - Path: res://DebugLogger.gd
##    - Node Name: DebugLogger
## 3. Restart the editor
## 4. The logger will automatically initialize when you run the game
##
## USAGE IN C++:
## - Use DBG_INFO(), DBG_WARN(), DBG_ERROR(), etc. macros
## - Logging automatically routes through the registered logger
##
## USAGE IN GDSCRIPT:
## var logger = DebugLogger.get_singleton()
## logger.set_log_level(DebugLogger.LogLevel.DEBUG)

func _ready() -> void:
	# Initialize and register the debug logger
	# The DebugLogger C++ class self-registers when instantiated
	# This creates the singleton instance
	initialize_debug_logger()

func initialize_debug_logger() -> void:
	# Call the static method to ensure logger is created and registered
	# The logger will print confirmation when initialized
	pass  # Logger is initialized by C++ code when first DBG_* macro is called
