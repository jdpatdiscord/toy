#include "runtime.hpp"

std::uintptr_t rbx_localscript_globalthread = NULL;
std::uintptr_t main_script_context = NULL;
std::uintptr_t data_model = NULL;

std::uintptr_t waiting_script_job_destroy = NULL;
std::uintptr_t* new_vft = NULL;

#pragma optimize("", off)
__declspec(naked) void __stdcall stdcall_empty_function()
{
	__asm ret;
}
__declspec(naked) void __fastcall fastcall_empty_function()
{
	__asm ret;
}
#pragma optimize("", on)

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

std::uint32_t RBX::Name::_impl_namehash(char* begin, char* end)
{
	unsigned int result;
	int v4;

	char* v2 = begin;
	for (result = 0; v2 != end; result ^= (result >> 2) + (result << 6) + v4)
		v4 = *v2++ - 0x61C88647;
	return result;
}

std::uintptr_t RBX::Name::_impl_find(std::uintptr_t map_0, const char** pkey)
{
	std::uintptr_t map1_4 = *(std::uintptr_t*)(map_0 + 4);
	std::uintptr_t map1_0 = *(std::uintptr_t*)(map_0 + 0);
	std::uintptr_t map2_0 = *(std::uintptr_t*)(map_0 + 0);
	std::uintptr_t map0_16 = *(std::uintptr_t*)(map_0 + 16);
	std::uintptr_t key_0 = (std::uintptr_t)(*pkey);
	std::uintptr_t key_1 = (std::uintptr_t)(*pkey);

	if (!strcmp((char*)key_0, (char*)map0_16)) return NULL;

	int keya = 0;

	std::uint32_t name_hash = _impl_namehash((char*)key_0, (char*)(key_0 + strlen((char*)key_0)));
	std::uint32_t hash_mask = ((map1_4 - map1_0) >> 3) - 1;
	std::uint32_t hash_total = hash_mask & name_hash;

	int sresult;

	while (1)
	{
		std::uintptr_t ppDescriptor = (std::uintptr_t)(map2_0 + 8 * hash_total);
		sresult = strcmp(*(const char**)(ppDescriptor + 0), (char*)key_1);
		if (sresult == 0)
		{
			return (std::uintptr_t)ppDescriptor + 4;
		}
		//sresult = strcmp(*(const char**)(ppDescriptor + 0), (char*)map0_16);
		//if (sresult)
		{
			hash_total = hash_mask & (keya + hash_total + 1);
			if (++keya <= hash_mask) continue;
		}
		return NULL;
	}
}

std::uintptr_t RBX::Name::Lookup(const char* classname, const char* output_file)
{
	std::uintptr_t namefactory = *(std::uintptr_t*)namefactory_singleton;

	int mutex_status = ((int(__cdecl*)(std::uintptr_t))namefactory_lock)(namefactory);
	if (mutex_status != 0) return 0;

	std::uintptr_t result = _impl_find(namefactory + o_namefactory_namemap, &classname);
	if (result)
		result = *(std::uintptr_t*)result;
	else
		result = namefactory + o_namefactory_nullname;

	((int(__cdecl*)(std::uintptr_t))namefactory_unlock)(namefactory);

	return result;
}

std::uintptr_t RL::decrypt_pointer(std::uintptr_t address, int type)
{
	switch (type)
	{
		case ptr_addptr:
			return *(std::uintptr_t*)address - address;
		case ptr_subptr:
			return address - *(std::uintptr_t*)address;
		case ptr_suboff:
			return *(std::uintptr_t*)address + address;
		case ptr_xorptr:
			return *(std::uintptr_t*)address ^ address;
	}
	return 0;
};

void RL::encrypt_pointer(std::uintptr_t address, std::uintptr_t value, int type)
{
	switch (type)
	{
		case ptr_addptr:
			*(DWORD*)address = address + value;
			break;
		case ptr_subptr:
			*(DWORD*)address = address - value;
			break;
		case ptr_suboff:
			*(DWORD*)address = value - address;
			break;
		case ptr_xorptr:
			*(DWORD*)address = address ^ value;
			break;
	}
};