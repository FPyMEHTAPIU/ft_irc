#pragma once
#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <algorithm>
#include "Server.hpp"
#include "client/Client.hpp"
#include "channel/Channel.hpp"

#define ANSI_RED "\033[0;91m"
#define ANSI_RESET "\033[0;39m"

#define PWD_MIN_LENGTH 5
#define PWD_MAX_LENGTH 32

class Server;

// parser.cpp
void validatePassword(const std::string &password);
int validateArgs(int argc, char **argv);
int validatePort(const std::string &strPort);
void validateClientName(std::string name);
void validateCommand(const std::string &command);

// utils/
bool isValidChannelName(const std::string &name);
std::vector<std::string> split(std::string sentence, char sep);

std::string handleInput(const std::string &input, Server *server, int clientFd);

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
