#include "Server.hpp"
#include "Logger.hpp"
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <iostream>
#include <set>
#include <string>
#include <algorithm> // for std::find_if
#include <sys/fcntl.h>

Server::Server(int port, std::string password, Logger *logger)
    : _PORT(port), _status(false), _PASSWORD(password), _serverSocket(-1), _logger(logger)
{
    if (!_logger)
    {
        throw std::invalid_argument("Logger cannot be null");
    }

    _logger->info(SERVER, "Initializing server on port " + std::to_string(port));
    setupSocket();
    bindSocket();
    listenSocket();
    _logger->info(SERVER, "Server initialization complete");
}

Server::~Server()
{
    stop();
}

std::map<std::string, std::shared_ptr<Channel>> Server::getChannels()
{
    return _channels;
}

std::map<int, std::shared_ptr<Client>> Server::getClients()
{
    return _clients;
}

std::vector<struct pollfd> Server::getPollFds() const
{
    return _pollFds;
}

const std::string &Server::getPassword() const
{
    return _PASSWORD;
}

std::shared_ptr<Client> Server::getClientByNick(const std::string &nick, const std::string &senderNick)
{
    for (auto &client : _clients)
    {
        if (client.second->getNick() == nick)
        {
            return client.second;
        }
    }
    throw std::invalid_argument(":ircserv 401 " + senderNick + " " + nick + " :No such nick/channel\r\n");
}

std::shared_ptr<Channel> Server::getChannelByName(const std::string &channelName)
{
    auto channel = _channels.find(channelName);
    if (channel != _channels.end())
    {
        return channel->second;
    }
    throw std::invalid_argument(":ircserv 401 " + channelName + " :No such nick/channel\r\n");
}

void Server::addChannel(const std::string &channelName, std::shared_ptr<Channel> channel)
{
    _channels.insert({channelName, channel});
}

void Server::addClient(int fd, Client client)
{
    _clients.insert({fd, std::make_shared<Client>(client)});
}

// Private socket methods
void Server::setupSocket()
{
    // Create socket with SOCK_NONBLOCK
    _serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // | SOCK_NONBLOCK, 0); commented for MacOS, doesn't work with nonblock

    if (_serverSocket == -1)
    {
        _logger->fatal(SERVER, "Failed to create socket: " + std::string(strerror(errno)));
        throw std::runtime_error("Socket creation failed");
    }

    _logger->debug(SERVER, "Socket created successfully");

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        _logger->error(SERVER, "Failed to set socket options: " + std::string(strerror(errno)));
        throw std::runtime_error("Socket option setting failed");
    }
}

void Server::bindSocket()
{
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(_PORT);

    if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
    {
        _logger->fatal(SERVER, "Failed to bind socket to port " + std::to_string(_PORT) + ": " + std::string(strerror(errno)));
        throw std::runtime_error("Socket binding failed");
    }

    _logger->info(SERVER, "Socket bound to port " + std::to_string(_PORT));
}

void Server::listenSocket()
{
    if (listen(_serverSocket, SERVER_BACKLOG) < 0)
    {
        _logger->fatal(SERVER, "Failed to listen on socket: " + std::string(strerror(errno)));
        throw std::runtime_error("Socket listen failed");
    }

    _logger->info(SERVER, "Server listening with backlog " + std::to_string(SERVER_BACKLOG));
}

void Server::start()
{
    _logger->info(SERVER, "Starting IRC server on port " + std::to_string(_PORT));

    struct pollfd serverPoll;
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    _pollFds.push_back(serverPoll);

    _logger->debug(SERVER, "Server socket added to poll monitoring");
}

void Server::run()
{
    std::cout << "IRC server is running..." << std::endl;
    _status = true;

    while (_status)
    {
        int pollRes = poll(_pollFds.data(), _pollFds.size(), 1000); // 1-second tick

        if (pollRes < 0)
            throw std::runtime_error("Poll failed: " + std::string(strerror(errno)));
        if (pollRes == 0)
            continue;

        for (auto &pfd : _pollFds)
        {
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
        {
            _logger->error(NETWORK, "Failed to accept client connection: " + std::string(strerror(errno)));
        }
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

    std::string clientIP = inet_ntoa(clientAddr.sin_addr);
    _logger->info(CLIENT, "New client connected. FD: " + std::to_string(clientSocket) + " IP: " + clientIP);

    send(clientSocket, "Welcome to our IRC server!\n", 29, 0);
}

void Server::handleClientData(int clientFd)
{
    char buffer[BUFFER_SIZE];

    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0); // attempt to read data

    // handle read errors and disconnections
    if (bytesRead <= 0)
    {
        // == 0  means client closed connection gracefully
        if (bytesRead == 0)
        {
            _logger->info(CLIENT, "Client FD " + std::to_string(clientFd) + " disconnected");
        }
        else if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            _logger->error(CLIENT, "Error reading from client FD " + std::to_string(clientFd) + ": " + std::string(strerror(errno)));
        }
        removeClient(clientFd);
        return;
    }

    buffer[bytesRead] = '\0';
    _logger->info(CLIENT, "Received from client fd " + std::to_string(clientFd) + ": " + buffer);

    std::vector<std::string> cmds = split(buffer, '\n');

    for (const std::string &cmd : cmds)
    {
        handleInput(cmd, this, this->_logger, clientFd);
    }
}

void Server::handleClientWrite(int fd)
{
    try
    {
        std::shared_ptr<Client> client = _clients.at(fd);

        if (!client->hasPendingMessages())
        {
            disableWrite(fd);
            return;
        }

        std::string &msg = client->frontMessage();
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
            client->popMessage();
        }
        else
        {
            msg.erase(0, sent);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception when handling client write: " << e.what() << std::endl;
    }
}

void Server::removeClient(int clientFd)
{
    _logger->info(CLIENT, "Removing client FD " + std::to_string(clientFd));

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

    std::cout << "IRC server stopped." << std::endl;
}
