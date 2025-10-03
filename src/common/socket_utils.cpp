#include "socket_utils.hpp"

void SocketUtils::enableWrite(std::vector<struct pollfd>& pollFds, int clientFd)
{
    for (size_t i = 0; i < pollFds.size(); ++i)
    {
        if (pollFds.at(i).fd == clientFd)
        {
            pollFds.at(i).events |= POLLOUT;
            break;
        }
    }
}

void SocketUtils::disableWrite(std::vector<struct pollfd>& pollFds, int fd)
{
    for (size_t i = 0; i < pollFds.size(); ++i)
    {
        if (pollFds.at(i).fd == fd)
        {
            pollFds.at(i).events &= ~POLLOUT;
            return;
        }
    }
}

void SocketUtils::removeFromPoll(std::vector<struct pollfd>& pollFds, int fd)
{
    auto it = std::find_if(pollFds.begin(), pollFds.end(),
                          [fd](const struct pollfd &pfd)
                          { return pfd.fd == fd; });
    if (it != pollFds.end())
    {
        pollFds.erase(it);
    }
}
