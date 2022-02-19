#include "general.hpp"
#include "addresses.hpp"
#include "commands.hpp"
#include "runtime.hpp"

FILE* stream;

ExceptionManager::EHSettings settings = {
	{ 0x80000004,
	  0x80000006,
	  0x40010006,
	  0x406D1388 },                                /* blacklisted codes*/
	{ },                                           /* blacklisted symbols */
	"Toy.dll",                                     /* program name std::optional */
	(std::uintptr_t)GetModuleHandle(NULL),         /* base */
	NULL,                                          /* attempts to get prog size for you if NULL */
	ExceptionManager::DefaultHandler,              /* report handler: what to do with the finished report */
	ExceptionManager::DefaultProcessor,            /* report parser: how to generate the finished report */
	NULL,                                          /* inbuilt report location */
	NULL,                                          /* inbuilt report size */
	true,                                          /* is this a DLL?: */
	true,                                          /* use SEH?: */
	false,                                         /* use VEH?: */
};

/* helpful functions */

PWCHAR unicodeCharinfoToWideString(PCHAR_INFO pch, DWORD dwElemCount)
{
	const size_t buffer_size = dwElemCount * sizeof(WCHAR);
	PWCHAR str = (PWCHAR)malloc(buffer_size);
	if (str != NULL)
	{
		memset(str, 0, buffer_size);
		for (unsigned c = 0; c < dwElemCount; ++c)
		{
			str[c] = pch[c].Char.UnicodeChar;
		}
	}
	return str;
}

BOOL copyUnicodeToClipboard(PWCHAR buffer, DWORD dwElemCount)
{
	const size_t buffer_size = dwElemCount * sizeof(*buffer);
	if (buffer != NULL)
	{
		HGLOBAL Ga = GlobalAlloc(GMEM_MOVEABLE, buffer_size + sizeof(*buffer));
		if (Ga != NULL)
		{
			PWCHAR g_buffer = (PWCHAR)GlobalLock(Ga);
			if (g_buffer != NULL)
			{
				wcscpy(g_buffer, buffer);
				GlobalUnlock(Ga);
				SetClipboardData(CF_UNICODETEXT, Ga);
			}
			GlobalFree(Ga);
			return (g_buffer != NULL);
		}
	}
	return FALSE;
}

template <typename T> void precon_split(const std::string& s, char delim, T result)
{
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		*result++ = item;
	}
}

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> elems;
	precon_split(s, delim, std::back_inserter(elems));
	return elems;
}

/* forward decls */
void setup_hooks();

INT WINAPI ctrl_handler(DWORD event)
{
	switch (event)
	{
	case CTRL_CLOSE_EVENT:
		// for some reason, this is handled but the parent window closes too (why)
		ShowWindow(::GetConsoleWindow(), SW_HIDE);
		return 1;
	case CTRL_C_EVENT:
		// give up because windows doesn't actually fire CTRL_C_EVENT if you have text selected when you Ctrl+C (?)
		return 1;
	}
	return 0;
}

void console_init()
{
	DWORD checkerValue;

	// The Windows functions here are `jmp ds:[0x????????]`
	// `jmp ds:[????????]` is encoded as `FF 25 ?? ?? ?? ??`
	// ROBLOX checks the first 2 bytes `FF 25` because they
	// are guaranteed; the pointer value is not. If the
	// pointer is replaced to an inert function such like
	// IsSystemResumeAutomatic, IsWow64Message, InSendMessage,
	// etc. then this basic detection is subverted.

	PBYTE replacementPtr = (PBYTE)&IsSystemResumeAutomatic;
	PBYTE destinationPtr = (PBYTE)&FreeConsole;
	VirtualProtect((PVOID)destinationPtr, 4, PAGE_EXECUTE_READWRITE, &checkerValue);
	*(DWORD*)(destinationPtr + 2) = *(DWORD*)(replacementPtr + 2);
	VirtualProtect((PVOID)destinationPtr, 4, checkerValue, &checkerValue);

	if (GetConsoleWindow() == NULL)
	{
		AllocConsole();
		SetConsoleTitleA("Toybox");
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);
		freopen_s(&stream, "CONIN$", "r", stdin);
		HWND ConsoleHandle = GetConsoleWindow();

		HANDLE stdHnd = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode;
		GetConsoleMode(stdHnd, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(stdHnd, dwMode);

		::SetWindowPos(ConsoleHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		::ShowWindow(ConsoleHandle, SW_NORMAL);

		SetConsoleCtrlHandler((PHANDLER_ROUTINE)(ctrl_handler), true);
	}
}

int number_of_children(std::uintptr_t instance)
{
	int num_children = 0;

	auto p_children_list = *(std::uintptr_t*)(instance + o_children);
	if (p_children_list)
	{
		auto p_children_ptr = *(std::uintptr_t*)(p_children_list + 0);
		auto p_children_end = *(std::uintptr_t*)(p_children_list + 4);
		for (; p_children_ptr != p_children_end; p_children_ptr += 8)
			++num_children;
	}

	return num_children;
}

void display_instance(std::uintptr_t instance)
{
	if (instance != NULL)
	{
		printf(
			     "instance address: 0x%08X"
			"\n" "==============================="
			"\n" "instance.Name = \"%s\""
			"\n" "instance.ClassName = \"%s\""
			"\n" "==============================="
			"\n" "# of children: %i"
			"\n"
			, instance
			, instance_name(instance).c_str()
			, instance_classname(instance)
			, number_of_children(instance)
		);
	}
	else
	{
		printf("such instance does not exist\n");
	}

	return;
}

int null_zero()
{
	return 0;
}

int stepped_signal()
{
	command_mtx.lock();
	for (auto v_index = command_buffer.begin(); v_index != command_buffer.end(); /* no iteration */)
	{
		auto full_command = *(v_index);

		auto& [keyword, arg_list] = full_command;
		if (auto command_spec = command_list.find(keyword); command_spec != command_list.end())
		{
			auto& [name, info] = (*command_spec);
			auto& [func, desc] = info;
			func(0, arg_list);
		}
		command_buffer.erase(v_index);
	}
	command_mtx.unlock();
	return 0;
}


int __fastcall teleport_callback(std::uintptr_t _this, std::uintptr_t, std::uintptr_t a1)
{
	printf("teleport\n");

	main_script_context = 0;
	data_model = 0;

	delete new_vft;

	std::thread T(setup_hooks);
	T.detach();

	return (*(int(__thiscall*)(std::uintptr_t, std::uintptr_t))waiting_script_job_destroy)(_this, a1);
}

void setup_hooks()
{
	while (!main_script_context)
	{
		printf("main_script_context == 0\n");

		job* waiting_script_job = nullptr;

		auto jobs_singleton = *(std::uintptr_t*)main_jobs_singleton;

		auto jobs_begin = *(std::uintptr_t*)(jobs_singleton + o_jobs_begin);
		auto jobs_end = *(std::uintptr_t*)(jobs_singleton + o_jobs_end);
		for (auto job_ptr = jobs_begin; job_ptr != jobs_end; job_ptr += 8)
		{
			std::uintptr_t ref_ptr = *(std::uintptr_t*)(job_ptr + 4);
			if (ref_ptr != NULL)
			{
				job* job_obj = *(job**)job_ptr;
				printf("name string: %s\n", job_obj->name.c_str());
				if (auto& job_name = job_obj->name; job_name == "WaitingHybridScriptsJob")
				{
					printf("found %s\n", job_name.c_str());
					waiting_script_job = job_obj;
				}
			}
		}

		if (waiting_script_job != nullptr)
		{
			if (waiting_script_job->vf_table != new_vft)
			{
				new_vft = new std::uintptr_t[20];
				memcpy(new_vft, waiting_script_job->vf_table, sizeof(std::uintptr_t[20]));

				waiting_script_job_destroy = new_vft[0];
				new_vft[0] = (std::uintptr_t)teleport_callback;

				waiting_script_job->vf_table = new_vft;
			}
			else
			{
				printf("waiting_script_job already hooked\n");
			}
		}
		else
		{
			printf("waiting_script_job doesn't exist\n");
			continue; // in while loop
		}

		main_script_context = *(std::uintptr_t*)((std::uintptr_t)waiting_script_job + o_waitingscriptjob_scriptcontext);

		data_model = *(std::uintptr_t*)(main_script_context + o_parent);
		if (data_model)
		{
			auto name = instance_classname(data_model);
			if (!strcmp(name, "DataModel")) /* verify that ScriptContext.Parent is the DataModel */
			{
				printf("found %s\n", name);

				auto p_children_list = *(std::uintptr_t*)(data_model + o_children);
				if (p_children_list) /* does the game have a children vector yet? */
				{
					/* instances are std::shared_ptr's, so iterate past them with += 8. */
					for (
						std::uintptr_t instance_index_ptr = *(std::uintptr_t*)p_children_list;
						instance_index_ptr != *(std::uintptr_t*)(p_children_list + 4); /* check if the current instance isn't the end of the list */
						instance_index_ptr += 8 /* skip past the std::shared_ptr */
						)
					{
						/* since what's in the lists are pointers to an instance, you must dereference */
						auto instance = *(std::uintptr_t*)instance_index_ptr;
						auto classname = instance_classname(instance);
						if (!strcmp(classname, "RunService"))
						{
							printf("found %s\n", classname);
							/* we want RunService to attach our command/script queue to it */

							auto signal_data = new std::uint32_t[8];
							signal_data[0] = 0;
							signal_data[1] = 1;
							signal_data[2] = (std::uint32_t)stepped_signal;
							signal_data[3] = (std::uint32_t)null_zero;
							signal_data[4] = 0;
							signal_data[5] = instance + o_runservice_stepped; // stepped_offset
							signal_data[6] = 0;
							signal_data[7] = 0;

							((int(__thiscall*)(std::uintptr_t, std::uint32_t*))add_signal_function)(instance + o_runservice_stepped, signal_data);

							printf("signal set, ready for commands.\n");
						}
						if (!strcmp(classname, "ScriptContext"))
						{
							printf("found %s\n", classname);

							rbx_localscript_globalthread = RL::decrypt_pointer(instance + o_scriptcontext_localscriptstate, e_scriptcontext_enc);

							printf("LocalScript state: 0x%08X\n", rbx_localscript_globalthread);
						}
					}
				}
			}
		}
	}

	return;
}

int toybox_main()
{
	ExceptionManager::Init(&settings);
	console_init();
	setup_hooks();
	commands_init();

	while (true)
	{
		std::string command_buf;
		std::getline(std::cin, command_buf);

		command_mtx.lock();

		auto cmd_list = split(command_buf, ' ');

		if (cmd_list.size() != 0)
		{
			auto command = cmd_list[0];
			cmd_list.erase(cmd_list.begin() + 0); // delete command, retain arg list
			command_buffer.push_back(std::make_pair(command, cmd_list));
		}

		command_mtx.unlock();
	}
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);

		std::thread T(toybox_main);
		T.detach();
	}
	return TRUE;
}