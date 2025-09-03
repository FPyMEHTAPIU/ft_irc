#include "Server.hpp"
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <iostream>
#include <set>
#include <string>
#include <algorithm> // for std::find_if
#include <sys/fcntl.h>

Server::Server(int port, std::string password) : _PORT(port), _status(false), _PASSWORD(password), _serverSocket(-1)
{
    setupSocket();
    bindSocket();
    listenSocket();
}

Server::~Server()
{
    stop();
}

// In Server.cpp
std::map<std::string, std::shared_ptr<Channel>> &Server::getChannels()
{
    return _channels;
}

const std::map<std::string, std::shared_ptr<Channel>> &Server::getChannels() const
{
    return _channels;
}

const std::map<int, Client> &Server::getClients() const
{
    return _clients;
}

std::map<int, Client> &Server::getClients()
{
    return _clients;
}

std::vector<struct pollfd> Server::getPollFds() const
{
    return _pollFds;
}

void Server::addChannel(const std::string &channelName, std::shared_ptr<Channel> channel)
{
    _channels.insert({channelName, channel});
}

void Server::addClient(int fd, Client client)
{
    _clients.insert({fd, client});
}

// Private socket methods
void Server::setupSocket()
{
    // Create socket with SOCK_NONBLOCK
    _serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // | SOCK_NONBLOCK, 0); commented for MacOS, doesn't work with nonblock

    if (_serverSocket == -1)
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));

    int opt = 1; // Set socket options to 1 to allow address reuse

    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(_serverSocket);
        throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
    }
}

void Server::bindSocket()
{
    _serverAddr.sin_family = AF_INET;         // ipv4
    _serverAddr.sin_addr.s_addr = INADDR_ANY; //  Listen on all available network interfaces
    _serverAddr.sin_port = htons(_PORT);      // Host TO Network Short, uses big-endian byte order

    if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
    {
        close(_serverSocket);
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }
}

void Server::listenSocket()
{
    if (listen(_serverSocket, SERVER_BACKLOG) < 0)
    {
        close(_serverSocket);
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }
}

void Server::start()
{
    std::cout << "IRC Server starting on port " << _PORT << std::endl;

    // Add server socket to poll array
    struct pollfd serverPoll;
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    _pollFds.push_back(serverPoll);
}

void Server::run()
{
    std::cout << "IRC Server is running..." << std::endl;
    _status = true;

    while (_status)
    {
        int pollRes = poll(_pollFds.data(), _pollFds.size(), 1000); // 1-second tick

        if (pollRes < 0)
            throw std::runtime_error("Poll failed: " + std::string(strerror(errno)));
        if (pollRes == 0)
            continue;

        size_t i = 0;
        while (i < _pollFds.size())
        {
            struct pollfd &pfd = _pollFds[i];

            // Handle incoming data
            if (pfd.revents & POLLIN)
            {
                if (pfd.fd == _serverSocket)
                    acceptNewClient();
                else
                {
                    handleClientData(pfd.fd);

                    // After handleClientData(), check if client was removed
                    if (std::find_if(_pollFds.begin(), _pollFds.end(),
                                     [&](const pollfd &pf)
                                     { return pf.fd == pfd.fd; }) == _pollFds.end())
                    {
                        continue; // client removed, skip increment
                    }
                }
            }

            // Handle outgoing messages
            if (pfd.revents & POLLOUT)
            {
                handleClientWrite(pfd.fd);
            }

            // Handle hangups/errors
            if (pfd.revents & (POLLHUP | POLLERR))
            {
                if (pfd.fd != _serverSocket)
                {
                    removeClient(pfd.fd);
                    _clients.erase(pfd.fd);
                    continue; // client removed, skip increment
                }
            }

            ++i;
        }
    }
}

void Server::acceptNewClient()
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
            std::cerr << "Failed to accept client: " << strerror(errno) << std::endl;
        return;
    }

    int flags = fcntl(clientSocket, F_GETFL, 0);
    if (flags == -1)
        flags = 0;
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

    struct pollfd clientPoll;
    clientPoll.fd = clientSocket;
    clientPoll.events = POLLIN;
    clientPoll.revents = 0;
    _pollFds.push_back(clientPoll);

    Client newClient(clientSocket);
    addClient(clientSocket, newClient);

    std::cout << "New client connected: FD " << clientSocket
              << " from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

void Server::handleClientData(int clientFd)
{
    char buffer[1024]; // TODO: define a constant for buffer size

    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0); // attempt to read data

    // handle read errors and disconnections
    if (bytesRead <= 0)
    {
        // == 0  means client closed connection gracefully
        if (bytesRead == 0)
            std::cout << "Client FD " << clientFd << " disconnected." << std::endl;
        else if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            // real error!
            std::cerr << "Error reading from client FD " << clientFd << ": "
                      << strerror(errno) << std::endl;
        }
        removeClient(clientFd);
        return;
    }
    buffer[bytesRead] = '\0';
    std::cout << "Received from client fd " << clientFd << ": " << buffer;

    std::string response = "";
    // handle command here
    if (!buffer[0])
        response = "Empty command provided\r\n";

    std::vector<std::string> cmds = split(buffer, '\n');

    for (const std::string &cmd : cmds)
    {
        response = handleInput(cmd, this, clientFd);
        send(clientFd, response.c_str(), response.length(), 0);
    }

    // response = handleInput(buffer, this, clientFd);

    // just echo back for now
    // std::string response = "Echo: " + std::string(buffer);
    // send(clientFd, response.c_str(), response.length(), 0);
}

void Server::handleClientWrite(int fd)
{
    Client &client = _clients.at(fd);

    if (!client.hasPendingMessages())
    {
        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            if (_pollFds[i].fd == fd)
            {
                _pollFds[i].events &= ~POLLOUT;
                break;
            }
        }
        return;
    }

    std::string &msg = client.frontMessage();
    ssize_t sent = send(fd, msg.c_str(), msg.size(), 0);

    if (sent < 0)
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            std::cerr << "Send error on fd " << fd << ": " << strerror(errno) << "\n";
            removeClient(fd);
        }
        return;
    }

    if ((size_t)sent == msg.size())
    {
        client.popMessage();
    }
    else
    {
        msg.erase(0, sent);
    }
}

void Server::removeClient(int clientFd)
{
    // Find the client socket in the pollfd vector
    auto it = std::find_if(_pollFds.begin(), _pollFds.end(), [clientFd](const struct pollfd &pfd)
                           { return pfd.fd == clientFd; });

    // If found, clean up the client
    if (it != _pollFds.end())
    {
        close(clientFd);
        _pollFds.erase(it); // remove from the vector
        std::cout << "Client FD " << clientFd << " removed." << std::endl;
    }
}

void Server::stop()
{
    if (_serverSocket != -1)
    {
        close(_serverSocket);
        _serverSocket = -1;
    }

    std::cout << "Closing all fds, except the server socket" << std::endl;
    for (size_t i = 1; i < _pollFds.size(); ++i)
        close(_pollFds[i].fd);

    _pollFds.clear();

    std::cout << "IRC Server stopped." << std::endl;
}