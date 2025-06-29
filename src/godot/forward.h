#pragma once
#include "core.h"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"


#define GODOT_BIND_PROP(cls, type, name, ...)						            \
{                                                                               \
	ClassDB::bind_method(D_METHOD("get_" #name), &cls::get_##name);	            \
	ClassDB::bind_method(D_METHOD("set_" #name, "p_" #name), &cls::set_##name); \
	auto prop_info = PropertyInfo(type, #name, ##__VA_ARGS__);                  \
	ADD_PROPERTY(prop_info, "set_" #name, "get_" #name);                        \
}

#define GODOT_PROP_GETTER(type, name)                                                 \
template <typename T> inline bool get_##name##_interceptor(T& value) { return true; } \
inline type get_##name() {					                                          \
	type value;																          \
	if (get_##name##_interceptor(value))									          \
		return m_##name; 						                                      \
	return value;															          \
}											                                          \

#define GODOT_PROP_SETTER(type, name)                                                       \
template <typename T> inline bool set_##name##_interceptor(const T& value) { return true; } \
inline void set_##name(type p_##name) {										                \
    if (set_##name##_interceptor(p_##name))									                \
	    m_##name = p_##name;												                \
};																			                \

namespace godot {

	inline static void CallDeferredImpl(uint64_t p_ObjID, std::uintptr_t p_FuncPtr) {
		auto func_ptr = reinterpret_cast<std::function<void()>*>(p_FuncPtr);
		assert(func_ptr && *func_ptr);
		if (auto obj = ObjectDB::get_instance(p_ObjID))
		{
			(*func_ptr)();
		}
		delete func_ptr;
	}

	inline static void CallDeferred(uint64_t p_ObjID, std::function<void()> p_Func)
	{
		callable_mp_static(&CallDeferredImpl).call_deferred(p_ObjID, std::uintptr_t(new std::function<void()>(std::move(p_Func))));
	}

}