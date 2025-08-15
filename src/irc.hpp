#pragma once
#include <iostream>
#include <set>
#include <bits/stdc++.h>

#define ANSI_RED "\033[0;91m"
#define ANSI_RESET "\033[0;39m"

#define PWD_MIN_LENGTH 5
#define PWD_MAX_LENGTH 32

// parser.cpp
void validatePassword(const std::string &password);
int validateArgs(int argc, char **argv);
int validatePort(const std::string &strPort);
void validateClientName(std::string name);
void validateCommand(const std::string &command);

// utils/
bool isValidChannelName(const std::string &name);