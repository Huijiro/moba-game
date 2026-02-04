extends Node

# Autoload script for registering the custom DebugLogger with Godot's OS
# Add this as an autoload in Project Settings > Autoload with name "DebugLogger"

func _init() -> void:
	# Create and register the custom logger
	var logger = DebugLogger.new()
	OS.add_logger(logger)
	print("DebugLogger registered with Godot")
