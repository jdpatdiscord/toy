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
const std::uintptr_t add_signal_function = _Rebase(0x1C9F540); // "AppStarted"/"GuiCluster"
const std::uintptr_t main_jobs_singleton = _Rebase(0x3739F9C); // "averageStepTime"

const std::uintptr_t call_ebx_occurrence = _Rebase(0x5E9CEF); // FF D3 in IDA

const std::uintptr_t luau_load = _Rebase(0x1A32710); // (lua_State* L, const char* chunkname, const char* data, size_t size, int env), "Parallel" xref
const std::uintptr_t rbx_spawn = _Rebase(0x7CA5E0); // __cdecl (lua_State*), "Spawn function requires 1 argument"

// "Unable to create an Instance of type"

const std::uintptr_t namefactory_singleton = _Rebase(0x36B91A4);

const std::uintptr_t namefactory_lock = _Rebase(0x213FFF5);
const std::uintptr_t namefactory_unlock = _Rebase(0x2140050);

/* modified offsets */

const std::ptrdiff_t o_namefactory_nullname = 72;
const std::ptrdiff_t o_namefactory_namemap = 48;

const std::ptrdiff_t o_waitingscriptjob_scriptcontext = 304;

const std::ptrdiff_t o_jobs_begin = 300;
const std::ptrdiff_t o_jobs_end = 304;

const std::ptrdiff_t o_scriptcontext_localscriptstate = 308;
const std::ptrdiff_t o_scriptcontext_corescriptstate = 508;

constexpr unsigned e_scriptcontext_enc = ptr_suboff;

const std::ptrdiff_t o_ls_top = 20;
const std::ptrdiff_t o_ls_base = 24;

const std::ptrdiff_t o_ls_extraspace = 112;

const std::ptrdiff_t o_es_identity = 24;

/* more or less constant values */
constexpr std::ptrdiff_t o_tt = 12;
constexpr std::ptrdiff_t c_tvaluesize = 16;

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