#pragma once

#include <Windows.h>

#include "ExceptionHandler/ExceptionHandler.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <vector>
#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <string>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <map>

#include <Luau/Compiler.h>
#include <Luau/Bytecode.h>
#include <Luau/BytecodeBuilder.h>

extern FILE* stream;

extern HMODULE this_dll;

const auto _this_module_base = GetModuleHandle(NULL);

#define _Rebase(N) (N - 0x400000 + (std::uintptr_t)_this_module_base)
#define _Unbase(N) (N - (std::uintptr_t)_this_module_base + 0x400000)

int number_of_children(std::uintptr_t instance);
void display_instance(std::uintptr_t instance);