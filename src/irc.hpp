#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include "server/Server.hpp"
#include "client/Client.hpp"
#include "channel/Channel.hpp"

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

// Validation functions now in validation namespace

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
