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
template <typename return_type, typename... targs> return_type ret_call(unsigned convention, std::uintptr_t address, targs... args)
{
	return_type return_value = 0;
	std::uint32_t esp_store = 0;
	
	__asm pushad;
	__asm mov esp_store, esp;

	if (convention == cc_fastcall)
	{
		((return_type(__fastcall*)(targs...))fastcall_empty_function)(args...);
		//__asm add esp, 0x4;
	}
	else
	{
		((return_type(__stdcall*)(targs...))stdcall_empty_function)(args...);
	}

	__asm
	{
		mov eax, finished_call;
		mov ebx, eax;
		push call_ebx_occurrence;
		jmp [address];
	finished_call:
		mov esp, esp_store;
		mov return_value, eax;
	}

	__asm popad;

	return return_value;
}
#pragma optimize("", on)

std::string& instance_name(std::uintptr_t instance);
char* instance_classname(std::uintptr_t instance);
std::uintptr_t find_first_child(std::uintptr_t instance, const char* name);
std::uintptr_t find_first_child_of_class(std::uintptr_t instance, const char* class_name);
std::uintptr_t find_first_descendant(std::uintptr_t instance, const char* name, std::uintptr_t o);
std::uintptr_t find_first_descendant_of_class(std::uintptr_t instance, const char* class_name, std::uintptr_t o);

std::string compile_script_cstr(char* script, size_t size);

namespace RL
{
	std::uintptr_t decrypt_pointer(std::uintptr_t address, int type);
	void encrypt_pointer(std::uintptr_t address, std::uintptr_t value, int type);

	std::uintptr_t gettop(std::uintptr_t thread);
	void setthreadidentity(std::uintptr_t thread, std::uint64_t identity);
	std::uintptr_t index2adr(std::uintptr_t thread, int index);
};