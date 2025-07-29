#pragma once
#include <iostream>

#define ANSI_RED "\033[0;91m"
#define ANSI_RESET "\033[0;39m"

std::string generateRandomId();
int validateArgs(int argc, char **argv);