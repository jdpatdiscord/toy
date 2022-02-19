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

void __fastcall fastcall_empty_function();
void __stdcall stdcall_empty_function();

#pragma optimize("", off)
template <typename return_type, typename... args> return_type ret_call(unsigned convention, std::uintptr_t address, args... args)
{
	return_type return_value = 0;
	std::uint32_t esp_store = 0;
	
	__asm pushad;
	__asm mov esp_store, esp;

	if (convention == cc_fastcall)
		((return_type(__fastcall*)(args...))fastcall_empty_function)(args...);
	else
		((return_type(__stdcall*)(args...))stdcall_empty_function)(args...);

	__asm
	{
		mov eax, finished_call;
		mov ebx, eax;
		push call_ebx_occurrence;
		jmp [address];
	finished_call:
		mov esp, esp_store;
		mov return_value, eax;
		popad;
	}

	return return_value;
}
#pragma optimize("", on)

std::string& instance_name(std::uintptr_t instance);
char* instance_classname(std::uintptr_t instance);
std::uintptr_t find_first_child(std::uintptr_t instance, const char* name);
std::uintptr_t find_first_child_of_class(std::uintptr_t instance, const char* class_name);
std::uintptr_t find_first_descendant(std::uintptr_t instance, const char* name, std::uintptr_t o);
std::uintptr_t find_first_descendant_of_class(std::uintptr_t instance, const char* class_name, std::uintptr_t o);

namespace RL
{
	std::uintptr_t decrypt_pointer(std::uintptr_t address, int type);
	void encrypt_pointer(std::uintptr_t address, std::uintptr_t value, int type);
}

namespace RBX
{
	namespace Name
	{
		std::uint32_t _impl_namehash(char* begin, char* end);
		std::uintptr_t _impl_find(std::uintptr_t map_0, const char** pkey);
		std::uintptr_t Lookup(const char* classname, const char* output_file);
	}
}