@tool

extends FlowFieldPathfindingMap


func _input(event: InputEvent) -> void:
	print("_input: ", event)

func _unhandled_input(event: InputEvent) -> void:
	print("_unhandled_input: ", event)
