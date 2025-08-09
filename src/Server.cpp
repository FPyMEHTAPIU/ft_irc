#include "Server.hpp"
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <iostream>
#include <set>
#include <string>
#include <algorithm>  // for std::find_if


Server::Server(int port, std::string password): _PORT(port),  _status(false),_PASSWORD(password), _serverSocket(-1) {
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

std::set<Client> Server::getClients() const {
  return _clients;
}

void Server::addChannel(Channel channel) {
  _channels.insert(channel);
}

void Server::addClient(Client client) {
  _clients.insert(client);
}

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

void Server::run() {
    std::cout << "IRC Server is running..." << std::endl;
    _status = true;

    while (_status == true) {
        int pollRes = poll(_pollFds.data(), _pollFds.size(), 1000); //tick 1000ms

        if (pollRes < 0) throw std::runtime_error("Poll failed: " + std::string(strerror(errno)));
        if (pollRes == 0) continue;

        size_t i = 0;
        while (i < _pollFds.size()) {
        // POOLIN - this means data is available to read
          if (_pollFds[i].revents & POLLIN) {
             if (_pollFds[i].fd == _serverSocket) 
                 acceptNewClient();
             else
                 handleClientData(_pollFds[i].fd);
          }

          // handle disconnections here
          // POLLHUP - Poll Hang Up -  client pressed ctrl+c or closed irc client
          // POLLERR - Poll Error - socket error occurred
          if (_pollFds[i].revents & (POLLHUP | POLLERR)) {
              if (_pollFds[i].fd != _serverSocket) {
                  removeClient(_pollFds[i].fd);
                  // TODO check if we need to adjust the index
                  continue;
              }
          }
          ++i;
        }
    }
}

void Server::acceptNewClient() {
    struct sockaddr_in clientAddr; // needed for accept()
    socklen_t clientAddrLen = sizeof(clientAddr); // needed for accept()
    
    // accept the pending connection
    int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    
    if (clientSocket < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            std::cerr << "Failed to accept client: " << strerror(errno) << std::endl; // real error
        }
        return; // If it was EWOULDBLOCK/EAGAIN, just return
    }
    
    // Add client to poll monitoring
    struct pollfd clientPoll;
    clientPoll.fd = clientSocket;
    clientPoll.events = POLLIN;
    clientPoll.revents = 0;
    _pollFds.push_back(clientPoll);
    
    std::cout << "New client connected: FD " << clientSocket 
              << " from " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

void Server::handleClientData(int clientFd) {
    char buffer[1024]; // TODO: define a constant for buffer size

   
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);  // attempt to read data
    
    // handle read errors and disconnections
    if (bytesRead <= 0) {
        // == 0  means client closed connection gracefully
        if (bytesRead == 0) std::cout << "Client FD " << clientFd << " disconnected." << std::endl;
        else if (errno != EWOULDBLOCK && errno != EAGAIN) {
            // real error!
            std::cerr << "Error reading from client FD " << clientFd << ": " 
                     << strerror(errno) << std::endl;
        }
        removeClient(clientFd);
        return;
    }
    buffer[bytesRead] = '\0';
    std::cout << "Received from client fd " << clientFd << ": " << buffer;
    
    // just echo back for now
    std::string response = "Echo: " + std::string(buffer);
    send(clientFd, response.c_str(), response.length(), 0);
}

void Server::removeClient(int clientFd) {
    // Find the client socket in the pollfd vector
    auto it = std::find_if(_pollFds.begin(), _pollFds.end(), [clientFd](const struct pollfd& pfd) {
            return pfd.fd == clientFd;
        });
    
    // If found, clean up the client
    if (it != _pollFds.end()) {
        close(clientFd); 
        _pollFds.erase(it); //remove from the vector
        std::cout << "Client FD " << clientFd << " removed." << std::endl;
    }
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
