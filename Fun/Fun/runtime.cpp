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
	auto p_string_class = *(std::uintptr_t**)(instance + o_classname) + 1;
	if (*(std::uintptr_t*)(*p_string_class + o_classnamelength) >= 16)
	{
		return *(char**)*p_string_class;
	}
	return (char*)*p_string_class;
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

std::uintptr_t RL::gettop(std::uintptr_t thread)
{
	return ( *(std::uintptr_t*)(thread + o_ls_top) - *(std::uintptr_t*)(thread + o_ls_base) ) >> 4;
}

void RL::setthreadidentity(std::uintptr_t thread, std::uint64_t identity)
{
	*(std::uint64_t*)(*(std::uintptr_t*)(thread + o_ls_extraspace) + o_es_identity) = identity;
}

std::uintptr_t RL::index2adr(std::uintptr_t thread, int index)
{
	if (index <= 0)
	{
		//retcall
	} 
	else
	{
		std::uintptr_t object = *(std::uintptr_t*)(thread + o_ls_base) + (c_tvaluesize * (index - 1));
		if (object < *(std::uintptr_t*)(thread + o_ls_top))
		{
			return object;
		}
		else
		{
			return 0; // CHANGE TO NILOBJ
		}
	}
}

std::string compile_script_cstr(char* script, size_t size)
{
	struct ClientBytecodeEncoder : Luau::BytecodeEncoder
	{
		uint8_t encodeOp(const std::uint8_t op) override
		{
			return op * 227;
		}
	};

	ClientBytecodeEncoder encoder;
	std::string bytecode = Luau::compile(std::string(script, size), {}, {}, &encoder);

	return bytecode;
}