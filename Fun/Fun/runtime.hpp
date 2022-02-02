#pragma once

#include "general.hpp"
#include "addresses.hpp"

std::string instance_name(std::uintptr_t instance);
char* instance_classname(std::uintptr_t instance);
std::uintptr_t find_first_child(std::uintptr_t instance, const char* name);
std::uintptr_t find_first_child_of_class(std::uintptr_t instance, const char* class_name);
std::uintptr_t find_first_descendant(std::uintptr_t instance, const char* name, std::uintptr_t o);
std::uintptr_t find_first_descendant_of_class(std::uintptr_t instance, const char* class_name, std::uintptr_t o);