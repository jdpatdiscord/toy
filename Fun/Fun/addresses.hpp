#pragma once

#include "general.hpp"

constexpr unsigned ptr_addptr = 0;
constexpr unsigned ptr_subptr = 1;
constexpr unsigned ptr_suboff = 2;
constexpr unsigned ptr_xorptr = 3;

constexpr unsigned cc_stdcall = 0;
constexpr unsigned cc_cdecl = 1;
constexpr unsigned cc_fastcall = 2;

/* modified addresses */
const std::uintptr_t add_signal_function = _Rebase(0x1DA6980); // "AppStarted"/"GuiCluster"
const std::uintptr_t main_jobs_singleton = _Rebase(0x3E47574); // "GetJobsInfo" -> "averageStepsPerSecond", "averageStepTime", ...

const std::uintptr_t call_ebx_occurrence = _Rebase(0x7035D3); // Search -> Sequence of bytes FF D3 in IDA

const std::uintptr_t luau_load = _Rebase(0x734E00); // (lua_State* L, const char* chunkname, const char* data, size_t size, int env), "challenge", "oldResult, moduleRef"... 
const std::uintptr_t rbx_spawn = _Rebase(0x7F8A10); // __cdecl (lua_State*), "Spawn function requires 1 argument" unverified

const std::uintptr_t walkspeed_setter = _Rebase(0xA29660);

// task.delay "delay function requires 2 arguments"

//

const std::uintptr_t namefactory_singleton = _Rebase(0x36B91A4); // "Unable to create an Instance of type"

const std::uintptr_t namefactory_lock = _Rebase(0x213FFF5);
const std::uintptr_t namefactory_unlock = _Rebase(0x2140050);

/* modified offsets */

const std::ptrdiff_t o_namefactory_nullname = 72;
const std::ptrdiff_t o_namefactory_namemap = 48;

//

const std::ptrdiff_t o_waitingscriptjob_scriptcontext = 304; // need to remembert this too

const std::ptrdiff_t o_jobs_begin = 308;
const std::ptrdiff_t o_jobs_end = 312;

const std::ptrdiff_t o_scriptcontext_localscriptstate = 476; // fflag next to "Script Start" -> others "startScript re-entrancy has exceeded 3", "Running Script \"%s\"", "%s %s detected as malicious.  %s will not run."
const std::ptrdiff_t o_scriptcontext_corescriptstate = 300;

constexpr unsigned e_scriptcontext_enc = ptr_addptr;

const std::ptrdiff_t o_ls_top = 20;
const std::ptrdiff_t o_ls_base = 24;

const std::ptrdiff_t o_ls_extraspace = 112;

const std::ptrdiff_t o_es_identity = 24;

/* more or less constant values */
constexpr std::ptrdiff_t o_tt = 12; 
constexpr std::ptrdiff_t c_tvaluesize = 16;

/* base instance */
const std::ptrdiff_t o_children = 40; //"ClearAllChildren"
const std::ptrdiff_t o_parent = 48; //"Part does not have the same parent as Humanoid"
const std::ptrdiff_t o_name = 36; //"Character cannot be changed as Player (%s) is being removed."
const std::ptrdiff_t o_classname = 12;
const std::ptrdiff_t o_classnamelength = 16;

/* players */
const std::ptrdiff_t o_localplayer = 344; // "LocalPlayer"/"No local Player to chat from"

/* player */
const std::ptrdiff_t o_character = 124; // "Character"

/* runservice */
const std::ptrdiff_t o_runservice_stepped = 308; // "Stepped", ""RenderStepped event can only be used from local scripts"

/* humanoid */
// ...

/* runtime */
extern std::uintptr_t rbx_localscript_globalthread;

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