#include "runtime.hpp"

std::uintptr_t main_script_context;
std::uintptr_t data_model;

std::uintptr_t waiting_script_job_destroy;
std::uintptr_t* new_vft;

char* instance_classname(std::uintptr_t instance)
{
	auto p_classname_f = *(std::uintptr_t*)instance + o_classname;

	auto string_class = (*(std::uintptr_t(**)(void))p_classname_f)();
	if (*(std::uintptr_t*)(string_class + o_classnamelength) >= 16)
	{
		return *(char**)string_class;
	}
	return (char*)string_class;
}

std::string& instance_name(std::uintptr_t instance)
{
	 return **(std::string**)(instance + o_name);
}

std::uintptr_t find_first_child(std::uintptr_t instance, const char* name)
{
	auto p_children_list = *(std::uintptr_t*)(instance + o_children);
	if (p_children_list)
	{
		auto p_children_ptr = *(std::uintptr_t*)(p_children_list + 0);
		auto p_children_end = *(std::uintptr_t*)(p_children_list + 4);
		for (; p_children_ptr != p_children_end; p_children_ptr += 8)
		{
			auto instance_child = *(std::uintptr_t*)p_children_ptr;
			std::string obj_name = instance_name(instance_child);
			if (obj_name == name)
			{
				return instance_child;
			}
		}
	}
	return 0;
}

std::uintptr_t find_first_child_of_class(std::uintptr_t instance, const char* class_name)
{
	auto p_children_list = *(std::uintptr_t*)(instance + o_children);
	if (p_children_list)
	{
		auto p_children_ptr = *(std::uintptr_t*)(p_children_list + 0);
		auto p_children_end = *(std::uintptr_t*)(p_children_list + 4);
		for (; p_children_ptr != p_children_end; p_children_ptr += 8)
		{
			auto instance = *(std::uintptr_t*)p_children_ptr;
			auto instance_class_name = instance_classname(instance);
			if (!strcmp(instance_class_name, class_name))
			{
				return instance;
			}
		}
	}
	return 0;
}

std::uintptr_t find_first_descendant(std::uintptr_t instance, const char* name, std::uintptr_t o)
{
	std::string instance_name = **(std::string**)(instance + o_name);
	if (instance_name == name) o = instance;

	auto p_children_list = *(std::uintptr_t*)(instance + o_children);
	if (p_children_list)
	{
		auto p_children_ptr = *(std::uintptr_t*)(p_children_list + 0);
		auto p_children_end = *(std::uintptr_t*)(p_children_list + 4);
		for (; p_children_ptr != p_children_end; p_children_ptr += 8)
		{
			auto c_instance = *(std::uintptr_t*)p_children_ptr;
			o = find_first_descendant(c_instance, name, o);
			if (o) return o;
		}
	}
	return o;
}

std::uintptr_t find_first_descendant_of_class(std::uintptr_t instance, const char* class_name, std::uintptr_t o)
{
	if (!strcmp(instance_classname(instance), class_name)) o = instance;

	auto p_children_list = *(std::uintptr_t*)(instance + o_children);
	if (p_children_list)
	{
		auto p_children_ptr = *(std::uintptr_t*)(p_children_list + 0);
		auto p_children_end = *(std::uintptr_t*)(p_children_list + 4);
		for (; p_children_ptr != p_children_end; p_children_ptr += 8)
		{
			auto c_instance = *(std::uintptr_t*)p_children_ptr;
			o = find_first_descendant_of_class(c_instance, class_name, o);
			if (o) return o;
		}
	}
	return o;
}