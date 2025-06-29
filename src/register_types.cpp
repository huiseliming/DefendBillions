#include "register_types.h"
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "core.h"
#include "godot/flow_field_pathfinding_map.h"

using namespace godot;

void gdextension_initializer(ModuleInitializationLevel p_level) {
	//godot::UtilityFunctions::print("initialize_module(", p_level ,")");
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	IModule::StartupModules();
	ClassDB::register_class<FlowFieldPathfindingMap>();
}

void gdextension_terminator(ModuleInitializationLevel p_level) {
	//godot::UtilityFunctions::print("uninitialize_module(", p_level ,")");
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	IModule::CleanupModules();
}

extern "C" {

GDExtensionBool GDE_EXPORT gdextension_entry(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(gdextension_initializer);
	init_obj.register_terminator(gdextension_terminator);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
	return init_obj.init();
}

}
