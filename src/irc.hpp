#pragma once
#include <iostream>

#define ANSI_RED "\033[0;91m"
#define ANSI_RESET "\033[0;39m"

std::string generateRandomId();

//parser.cpp
int validateArgs(int argc, char **argv);

//utils.cpp
bool isValidChannelName(const std::string& name);
void validateClientName(std::string name);