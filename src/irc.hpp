#pragma once
#include <iostream>

std::string generateRandomId();
bool validateArgs(int argc, char **argv);
bool parser(int port, std::string password);