#pragma once

#include "general.hpp"

/* modified addresses */
const std::uintptr_t add_signal_function = _Rebase(0x1C59A90); // "AppStarted"/"GuiCluster"
const std::uintptr_t main_jobs_singleton = _Rebase(0x36C15E4); // "averageStepTime"

const std::uintptr_t walkspeed_setter = _Rebase(0xBE7EA0); // int(instance*, char, float)
const std::uintptr_t walkspeed_getter = _Rebase(0xBE3820); // double(instance*)
const std::uintptr_t gravity_setter = _Rebase(0x94DB50); // int(instance*, char, float)
const std::uintptr_t gravity_getter = _Rebase(0x94DB40); // double(instance*)

const std::uintptr_t lookup_f = _Rebase(0x6ED6A0); // __thiscall int(char*)

/* modified offsets */
const std::ptrdiff_t o_waitingscriptjob_scriptcontext = 304;

const std::ptrdiff_t o_jobs_begin = 300;
const std::ptrdiff_t o_jobs_end = 304;

/* base instance */
const std::ptrdiff_t o_children = 44;
const std::ptrdiff_t o_parent = 52;
const std::ptrdiff_t o_name = 40;
const std::ptrdiff_t o_classname = 16;
const std::ptrdiff_t o_classnamelength = 20;

/* players */
const std::ptrdiff_t o_localplayer = 320; // "LocalPlayer"/"No local Player to chat from"

/* player */
const std::ptrdiff_t o_character = 124; // "Character"

/* runservice */
const std::ptrdiff_t o_runservice_stepped = 256; // "RunService.Stepped"

/* humanoid */
// ...

/* runtime */
extern std::uintptr_t  main_script_context;
extern std::uintptr_t  data_model;

extern std::uintptr_t  waiting_script_job_destroy;
extern std::uintptr_t* new_vft;

struct job
{
	void* vf_table; // 0
	int unk_0;
	int unk_1;
	int unk_2;
	const std::string name;
};