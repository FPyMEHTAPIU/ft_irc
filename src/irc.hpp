#pragma once
#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <cstring>
#include <algorithm>
#include "Server.hpp"
#include "client/Client.hpp"
#include "channel/Channel.hpp"
#include "Logger.hpp"

#define ANSI_RED "\033[0;91m"
#define ANSI_RESET "\033[0;39m"

#define PASSWORD_MIN_LENGTH 5
#define PASSWORD_MAX_LENGTH 32

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

class Server;

// parser.cpp
void validateClientPassword(const std::string &inputPassword, const std::string &storedPassword);
void validatePassword(const std::string &password);
int validateArgs(int argc, char **argv);
int validatePort(const std::string &strPort);

// utils/
bool isValidChannelName(const std::string &name);
bool isValidNickname(const std::string &nickName);
bool isValidUsername(const std::string &userName);
void validateClientName(std::string name);
void validateCommand(const std::string &command);

// utils.cpp
std::vector<std::string> split(const std::string &sentence, char sep);
std::string strToLowercase(const std::string &str);
void handleInput(std::string input, Server *server, Logger *logger, int clientFd);

// These `hash` fucntions are for using `std::string` in `switch`
constexpr uint32_t hash(const char *data, size_t size) noexcept
{
    uint32_t h = 5381;
    for (const char *c = data; c < data + size; ++c)
        h = ((h << 5) + h) + static_cast<unsigned char>(*c);
    return h;
}

// These `hash` fucntions are for using `std::string` in `switch`
constexpr uint32_t hash(const char *str) noexcept
{
    size_t len = 0;
    while (str[len] != '\0')
        ++len;
    return hash(str, len);
}
