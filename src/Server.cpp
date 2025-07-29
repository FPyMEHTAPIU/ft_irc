#include "Server.hpp"
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <iostream>
#include <set>
#include <string>


Server::Server(int port, std::string password): _PORT(port), _PASSWORD(password), _serverSocket(-1) {
    setupSocket();
    bindSocket();
    listenSocket();
}

Server::~Server() {
    stop();
}

std::set<Channel> Server::getChannels() const {
  return _channels;
}

// std::set<Client> Server::getClients() const {
//   return _clients;
// }

void Server::addChannel(Channel channel) {
  _channels.insert(channel);
}

// void Server::addClient(Client client) {
//   _clients.insert(client);
// }

// Private socket methods
void Server::setupSocket() {
    // Create socket with SOCK_NONBLOCK
    _serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (_serverSocket == -1) throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));

    int opt = 1; // Set socket options to 1 to allow address reuse

    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
    }

}

void Server::bindSocket() {
    _serverAddr.sin_family = AF_INET; // ipv4
    _serverAddr.sin_addr.s_addr = INADDR_ANY; //  Listen on all available network interfaces
    _serverAddr.sin_port = htons(_PORT); // Host TO Network Short, uses big-endian byte order

    if (bind(_serverSocket, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Failed to bind socket: " + std::string(strerror(errno)));
    }
}

void Server::listenSocket() {
    if (listen(_serverSocket, SERVER_BACKLOG) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Failed to listen on socket: " + std::string(strerror(errno)));
    }
}

void Server::start() {
    std::cout << "IRC Server starting on port " << _PORT << std::endl;
    
    // Add server socket to poll array
    struct pollfd serverPoll;
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;
    _pollFds.push_back(serverPoll);
}


void Server::stop() {
    if (_serverSocket != -1) {
        close(_serverSocket);
        _serverSocket = -1;
    }

    std::cout <<  "Closing all fds, except the server socket" << std::endl;
    for (size_t i = 1; i < _pollFds.size(); ++i)
        close(_pollFds[i].fd);

    _pollFds.clear();
    
    std::cout << "IRC Server stopped." << std::endl;
}



