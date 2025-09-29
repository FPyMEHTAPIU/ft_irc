#pragma once
#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <cstring>
#include <algorithm>
#include "./server/Server.hpp"
#include "./client/Client.hpp"
#include "./channel/Channel.hpp"
#include "./logger/Logger.hpp"

#define ANSI_RED "\033[0;91m"
#define ANSI_ORANGE "\e[0m\e[38;5;202m"
#define ANSI_YELLOW "\033[0;93m"
#define ANSI_GREEN "\033[0;92m"
#define ANSI_BLUE "\e[0m\e[38;5;39m"
#define ANSI_PURPLE "\e[0m\e[38;5;171m"
#define ANSI_RESET "\033[0;39m"

#define PASSWORD_MIN_LENGTH 5
#define PASSWORD_MAX_LENGTH 32

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

class Server;

struct messageInfo
{
    std::string target;
    std::shared_ptr<Client> sender;
    std::string senderNick;
    int senderFd;
    std::string message;
    bool isCommand;
};

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
void handleInput(std::string input, Server *server, int clientFd);
std::vector<std::string> trimSplitInput(std::string &input, std::string &msg);
std::string generatePrefix(std::shared_ptr<Client> client);

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
