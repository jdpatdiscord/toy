#include "commands.hpp"

std::map<std::string, const command_info> command_list;

std::mutex command_mtx;
std::vector<std::pair<std::string, std::vector<std::string>>> command_buffer;

std::uintptr_t cached_instance;

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

void commands_init()
{
	command_list.emplace("find_instance_of_name", command_info(cmd_get_instance_of_name, "Looks for an instance of Name passed in 1st argument, then prints the address. Takes one argument."));
	command_list.emplace("find_instance_of_classname", command_info(cmd_get_instance_of_classname, "Looks for an instance of ClassName passed in 1st argument, then prints the address. Takes one argument."));

	return;
};