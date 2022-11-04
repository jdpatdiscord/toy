#pragma once

#include "general.hpp"
#include "addresses.hpp"
#include "runtime.hpp"

/* command spec */
struct command_info
{
	std::function<void(int, std::vector<std::string>&)> c_func;
	std::string c_description;
};

void commands_init();

extern std::map<std::string, const command_info> command_list;

extern std::mutex command_mtx;
extern std::vector<std::pair<std::string, std::vector<std::string>>> command_buffer;

extern std::uintptr_t cached_instance;

/* command declarations */
void cmd_help(int, std::vector<std::string>& arg_list);
void cmd_get_instance_of_classname(int, std::vector<std::string>& arg_list);
void cmd_get_instance_of_name(int, std::vector<std::string>& arg_list);
void cmd_set_walkspeed(int, std::vector<std::string>& arg_list);
void cmd_get_localplayer(int, std::vector<std::string>& arg_list);
void cmd_get_properties_of_classname(int, std::vector<std::string>& arg_list);
void cmd_run_bytecode_file(int, std::vector<std::string>& arg_list);
void cmd_run_script_file(int, std::vector<std::string>& arg_list);
void cmd_autolaunch_experiment(int, std::vector<std::string>& arg_list);