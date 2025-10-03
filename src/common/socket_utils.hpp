#pragma once
#include <vector>
#include <poll.h>
#include "../irc.hpp"

class SocketUtils
{
public:
    static void enableWrite(std::vector<struct pollfd> &pollFds, int clientFd);
    static void disableWrite(std::vector<struct pollfd> &pollFds, int fd);
    static void removeFromPoll(std::vector<struct pollfd> &pollFds, int fd);
};
