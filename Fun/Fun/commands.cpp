#include "commands.hpp"

std::map<std::string, const command_info> command_list;

std::mutex command_mtx;
std::vector<std::pair<std::string, std::vector<std::string>>> command_buffer;

std::uintptr_t cached_instance;

void cmd_help(int, std::vector<std::string>& arg_list)
{
	for (auto& command : command_list)
	{
		auto& [name, info] = command;
		auto& [func, desc] = info;
		printf("%s: %s\n", name.c_str(), desc.c_str());
	}
}

void cmd_get_instance_of_classname(int, std::vector<std::string>& arg_list)
{
	if (arg_list.size() != 0)
	{
		if (data_model != NULL)
		{
			std::uintptr_t instance = find_first_descendant(data_model, arg_list[0].c_str(), NULL);
			display_instance(instance);
		}
	}
}

void cmd_get_instance_of_name(int, std::vector<std::string>& arg_list)
{
	if (arg_list.size() != 0)
	{
		if (data_model != NULL)
		{
			std::uintptr_t instance = find_first_descendant(data_model, arg_list[0].c_str(), NULL);
			display_instance(instance);
		}
	}
}

void cmd_set_walkspeed(int, std::vector<std::string>& arg_list)
{
	if (arg_list.size() != 0)
	{
		if (data_model != NULL)
		{
			std::uintptr_t players = find_first_child_of_class(data_model, "Players");
			if (players != NULL)
			{
				std::uintptr_t localplayer = *(std::uintptr_t*)(players + o_localplayer);
				if (localplayer != NULL)
				{
					std::uintptr_t character = *(std::uintptr_t*)(localplayer + o_character);
					if (character != NULL)
					{
						std::uintptr_t humanoid = find_first_child_of_class(character, "Humanoid");
						if (humanoid != NULL)
						{
							float new_speed = std::stof(arg_list[0]);
							//((int(__thiscall*)(std::uintptr_t, float))walkspeed_setter)(humanoid, new_speed);
						}
					}
				}
			}
		}
	}
}

void cmd_get_localplayer(int, std::vector<std::string>& arg_list)
{
	if (data_model != NULL)
	{
		std::uintptr_t players = find_first_child_of_class(data_model, "Players");
		std::uintptr_t localplayer = *(std::uintptr_t*)(players + o_localplayer);
		display_instance(localplayer);
	}
}

void cmd_get_properties_of_classname(int, std::vector<std::string>& arg_list)
{
	if (arg_list.size() != 0)
	{
		if (data_model != NULL)
		{
			std::uintptr_t instance = find_first_descendant_of_class(data_model, arg_list[0].c_str(), NULL);
			if (instance != NULL)
			{
				std::uintptr_t unk_1 = *(std::uintptr_t*)(instance + 12);
				std::uintptr_t prop_begin = *(std::uintptr_t*)(unk_1 + 536);
				std::uintptr_t prop_end = *(std::uintptr_t*)(unk_1 + 540);
				for (std::uintptr_t i = prop_begin; i < prop_end; i += 12)
				{
					unsigned val = *(unsigned*)(i + 4);
					if (val != NULL)
					{
						unsigned prop_id = *(unsigned*)(i + 4 + 4);
						bool is_thread_safe = false;
						switch (prop_id)
						{
							case 0:
							{
								std::uintptr_t unk_2 = *(std::uintptr_t*)(i + 4);
								if (unk_2 != NULL)
								{
									is_thread_safe = *(unsigned*)(unk_2 + 16) != 1;
								}
								break;
							}
							// The other cases involve Lua.
						}
						std::string& prop_name = **(std::string**)(*(std::uintptr_t*)(i + 4) + 4);
						printf("Property Name, ID: %s, %s\n", property_id_name[prop_id], prop_name.c_str());
					}
				}
			}
		}
	}
}

void cmd_lookup_name(int, std::vector<std::string>& arg_list)
{
	if (arg_list.size() != 0)
	{
		printf("Descriptor: 0x%08X\n", RBX::Name::Lookup(arg_list[0].c_str(), NULL));
	}
}

void cmd_get_descriptor_list(int, std::vector<std::string>& arg_list)
{
	if (arg_list.size() != 0)
	{
		RBX::Name::Lookup("ThisNameDoesNotExist", arg_list[0].c_str()); // force entire map traversal, and output to file
	}

	return;
}

void cmd_run_bytecode(int, std::vector<std::string>& arg_list)
{
	if (arg_list.size() != 0)
	{
		std::ifstream F(arg_list[0], std::ios::binary);
		size_t Fs = std::filesystem::file_size(arg_list[0]);
		char* Fb = (char*)malloc(Fs);
		F.read(Fb, Fs);
		F.close();

		if (rbx_localscript_globalthread != NULL && data_model != NULL)
		{
			ret_call<int, std::uintptr_t, char*, char*, int, std::uintptr_t>(
				cc_cdecl, luau_load, 
				rbx_localscript_globalthread, (char*)"@LOL", Fb, Fs, 0);
			ret_call<int, int>(
				cc_cdecl, rbx_spawn,
				rbx_localscript_globalthread);
		}
	}
}

void commands_init()
{
	command_list.emplace("help", command_info(cmd_help, "Lists all commands and their descriptions"));
	command_list.emplace("find_instance_of_name", command_info(cmd_get_instance_of_name, "Looks for an instance of Name passed in 1st argument, then prints the address. Takes one argument."));
	command_list.emplace("find_instance_of_classname", command_info(cmd_get_instance_of_classname, "Looks for an instance of ClassName passed in 1st argument, then prints the address. Takes one argument."));
	//command_list.emplace("set_walkspeed", command_info(cmd_set_walkspeed, "Sets LocalPlayer WalkSpeed"));
	command_list.emplace("get_localplayer", command_info(cmd_get_localplayer, "Displays LocalPlayer instance"));
	command_list.emplace("get_properties_of_classname", command_info(cmd_get_properties_of_classname, "Displays properties of a ClassName. The passed ClassName must exist within the game"));
	command_list.emplace("lookup_name", command_info(cmd_lookup_name, "Looks up ClassName"));
	command_list.emplace("get_descriptor_list", command_info(cmd_get_descriptor_list, "Gets list of class descriptors, and outputs to passed filename."));
	command_list.emplace("run_bytecode_file", command_info(cmd_run_bytecode, "Runs bytecode from a file."));

	return;
};