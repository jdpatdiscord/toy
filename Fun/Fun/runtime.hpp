#pragma once

#include "general.hpp"
#include "addresses.hpp"

static const char* const property_id_name[] = {
	"DescriptorType_Property",
	"DescriptorType_Event",
	"DescriptorType_Function",
	"DescriptorType_YieldFunction",
	"DescriptorType_Callback"
};

std::string& instance_name(std::uintptr_t instance);
char* instance_classname(std::uintptr_t instance);
std::uintptr_t find_first_child(std::uintptr_t instance, const char* name);
std::uintptr_t find_first_child_of_class(std::uintptr_t instance, const char* class_name);
std::uintptr_t find_first_descendant(std::uintptr_t instance, const char* name, std::uintptr_t o);
std::uintptr_t find_first_descendant_of_class(std::uintptr_t instance, const char* class_name, std::uintptr_t o);
std::uintptr_t new_instance(const char* classname, std::uintptr_t parent);

namespace RBX
{
	namespace Name
	{
		std::uint32_t _impl_namehash(char* begin, char* end);
		std::uintptr_t _impl_find(std::uintptr_t map_0, const char** pkey, void* special_arg);
		std::uintptr_t Lookup(const char* classname, const char* output_file);
	}
}