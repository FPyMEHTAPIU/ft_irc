#include "Server.hpp"
#include "../logger/Logger.hpp"
#include "../commands/commands.hpp"
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <iostream>
#include <set>
#include <string>
#include <algorithm> // for std::find_if
#include <sys/fcntl.h>
#include <termios.h>
#include <csignal>

volatile bool Server::_terminate = false;

Server::Server(int port, std::string password, std::shared_ptr<Logger> logger)
    : _PORT(port), _PASSWORD(password), _serverSocket(-1), logger(logger)
{
    if (!logger)
    {
        throw std::invalid_argument("Logger cannot be null");
    }

    setupSignals(true);

    logger->info(SERVER, "Initializing server on port " + std::to_string(port));
    setupSocket();
    bindSocket();
    listenSocket();
    logger->info(SERVER, "Server initialization complete");
}

Server::~Server()
{
    setupSignals(false);
    stop();
}

const std::map<std::string, std::shared_ptr<Channel>> &Server::getChannels() const
{
    return _channels;
}

const std::map<int, std::shared_ptr<Client>> &Server::getClients() const
{
    return _clients;
}

const std::vector<struct pollfd> &Server::getPollFds() const
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
    throw std::invalid_argument(":ircserv 401 " + senderNick + " " + nick + " :No such nick\r\n");
}

std::shared_ptr<Channel> Server::getChannelByName(const std::string &channelName)
{
    auto channel = _channels.find(channelName);
    if (channel != _channels.end())
    {
        return channel->second;
    }
    throw std::invalid_argument(":ircserv 403 " + channelName + " :No such channel\r\n");
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
        logger->fatal(SERVER, "Failed to create socket: " + std::string(strerror(errno)));
        throw std::runtime_error("Socket creation failed");
    }

    logger->debug(SERVER, "Socket created successfully");

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        logger->error(SERVER, "Failed to set socket options: " + std::string(strerror(errno)));
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
        logger->fatal(SERVER, "Failed to bind socket to port " + std::to_string(_PORT) + ": " + std::string(strerror(errno)));
        throw std::runtime_error("Socket binding failed");
    }

    logger->info(SERVER, "Socket bound to port " + std::to_string(_PORT));
}

void Server::listenSocket()
{
    if (listen(_serverSocket, SERVER_BACKLOG) < 0)
    {
        logger->fatal(SERVER, "Failed to listen on socket: " + std::string(strerror(errno)));
        throw std::runtime_error("Socket listen failed");
    }

    logger->info(SERVER, "Server listening with backlog " + std::to_string(SERVER_BACKLOG));
}

void Server::setupSignals(bool serverStart) noexcept
{
    static termios newTerminal;
    static termios oldTerminal;

    if (serverStart)
    {
        signal(SIGINT, Server::signalHandler);
        signal(SIGQUIT, Server::signalHandler);
        tcgetattr(STDIN_FILENO, &oldTerminal);
        newTerminal = oldTerminal;
        newTerminal.c_lflag &= ~ECHOCTL;
        oldTerminal.c_lflag |= ECHOCTL;
        tcsetattr(STDIN_FILENO, TCSANOW, &newTerminal);
    }
    else
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminal);
    }
}

void Server::signalHandler(int signum)
{
    if (signum == SIGQUIT || signum == SIGINT)
        Server::_terminate = true;
}

void Server::start()
{
    logger->info(SERVER, "Starting IRC server on port " + std::to_string(_PORT));

    struct pollfd serverPoll;
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    _pollFds.push_back(serverPoll);

    logger->debug(SERVER, "Server socket added to poll monitoring");
}

void Server::run()
{
    logger->success(SERVER, "IRC server running on port " + std::to_string(_PORT));

    while (!_terminate)
    {
        int pollRes = poll(_pollFds.data(), _pollFds.size(), 1000); // 1-second tick

        if (pollRes < 0)
        {
            if (errno == EINTR)
            {
                logger->info(SERVER, "Received shutdown signal");
                break;
            }
            logger->error(SERVER, "Poll error: " + std::string(strerror(errno)));
            continue;
        }

        if (pollRes == 0)
            continue; // No events occurred

        std::vector<struct pollfd> _pollFdsCopy = _pollFds; // Copy to avoid issues during iteration
        for (const auto &pfd : _pollFdsCopy)
        {
            // Check for incoming data
            if (pfd.revents & POLLIN)
            {
                if (pfd.fd == _serverSocket)
                    acceptNewClient();
                else
                    handleClientData(pfd.fd);
            }

            // Check for outgoing data
            if (pfd.revents & POLLOUT)
            {
                handleClientWrite(pfd.fd);
            }

            // Check for hangups or errors
            if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL))
            {
                if (pfd.fd != _serverSocket)
                {
                    removeClient(pfd.fd);
                    _clients.erase(pfd.fd);
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
            logger->error(NETWORK, "Failed to accept client connection: " + std::string(strerror(errno)));
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
    _pollFds.push_back(clientPoll);

    Client newClient(clientSocket);
    addClient(clientSocket, newClient);

    std::string clientIP = inet_ntoa(clientAddr.sin_addr);
    logger->info(CLIENT, "New client connected. FD: " + std::to_string(clientSocket) + " IP: " + clientIP);

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
            logger->info(CLIENT, "Client FD " + std::to_string(clientFd) + " disconnected");
        }
        else if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            logger->error(CLIENT, "Error reading from client FD " + std::to_string(clientFd) + ": " + std::string(strerror(errno)));
        }
        removeClient(clientFd);
        return;
    }

    buffer[bytesRead] = '\0';

    std::shared_ptr<Client> client = _clients.at(clientFd);

    logger->info(CLIENT, "Received from client fd " + std::to_string(clientFd) + ": " + buffer);

    // Append the received data to the buffer
    client->getBuffer().append(buffer);

    for (std::string::size_type pos; (pos = client->getBuffer().find("\n")) != std::string::npos;)
    {
        std::string message = client->getBuffer().substr(0, pos);
        client->getBuffer().erase(0, pos + 1);

        // Process the complete command
        std::vector<std::string> cmds = split(message, '\n');
        for (const std::string &cmd : cmds)
        {
            if (!cmd.empty()) // Skip empty commands
            {
                handleInput(cmd, this, clientFd);
            }
        }
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
        std::cout << "SENDING: " << msg << std::endl;
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
    logger->info(CLIENT, "Removing client FD " + std::to_string(clientFd));

    // Close the socket
    close(clientFd);

    // Remove from pollfd list
    auto it = std::find_if(_pollFds.begin(), _pollFds.end(),
                           [clientFd](const struct pollfd &pfd)
                           { return pfd.fd == clientFd; });
    if (it != _pollFds.end())
    {
        _pollFds.erase(it);
    }

    // Remove from clients map
    auto clientIt = _clients.find(clientFd);
    if (clientIt != _clients.end())
    {
        _clients.erase(clientIt);
    }

    std::cout << "Client FD " << clientFd << " fully removed." << std::endl;
}

void Server::removeChannel(std::string channelName)
{
    logger->info(CLIENT, "Removing channel " + channelName);
    _channels.erase(channelName);
}

void Server::stop()
{
    if (_serverSocket != -1)
    {
        close(_serverSocket);
        _serverSocket = -1;
    }

    logger->debug(SERVER, "Closing all fds, except the server socket");
    for (size_t i = 1; i < _pollFds.size(); ++i)
        close(_pollFds[i].fd);

    _pollFds.clear();

    logger->success(SERVER, "Successfully shut down IRC server.");
}
