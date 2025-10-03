#pragma once
#include "../irc.hpp"
#include <string>

namespace Validation
{
    // Password validation
    void validatePassword(const std::string &password);
    void validateClientPassword(const std::string &inputPassword, const std::string &storedPassword);

    // Client name validation
    bool isValidNickname(const std::string &nickName);
    bool isValidUsername(const std::string &userName);
    void validateClientName(std::string name);

    // Channel validation
    bool isValidChannelName(const std::string &name);

    // Command validation
    void validateCommand(const std::string &command);

    // Port validation
    int validatePort(const std::string &strPort);
    int validateArgs(int argc, char **argv);
}
