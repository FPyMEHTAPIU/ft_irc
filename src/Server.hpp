#pragma once
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "channel/Channel.hpp"
#include "client/Client.hpp"
#include "irc.hpp"
#include "Logger.hpp"

#define SERVER_BACKLOG 10 // max number of pending connections
#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK 2
#endif

class Channel;

class Server
{
private:
  const int _PORT;
  bool _status;
  const std::string _PASSWORD;

  int _serverSocket;
  Logger *_logger;

  std::map<std::string, std::shared_ptr<Channel>> _channels;
  std::map<int, Client> _clients;

  struct sockaddr_in _serverAddr;
  std::vector<struct pollfd> _pollFds;

  void setupSocket();
  void bindSocket();
  void listenSocket();

public:
  Server() = delete;
  Server &operator=(Server const &server) = delete;
  Server(int port, std::string password, Logger *logger);
  ~Server();

  void start();
  void run();
  void stop();

  void acceptNewClient();
  void handleClientData(int clientFd);
  void handleClientWrite(int fd);
  void removeClient(int clientFd);

  // Server getters
  std::map<std::string, std::shared_ptr<Channel>> &getChannels();
  const std::map<std::string, std::shared_ptr<Channel>> &getChannels() const;
  const std::map<int, Client> &getClients() const;
  std::map<int, Client> &getClients();
  std::vector<struct pollfd> getPollFds() const;
  const std::string &getPassword() const;
  Client &getClientByNick(const std::string &nick, const std::string &senderNick);

  void addChannel(const std::string &channelName, std::shared_ptr<Channel> channel);
  void addClient(int fd, Client client);

  void enableWrite(int clientFd)
  {
    for (size_t i = 0; i < _pollFds.size(); ++i)
    {
      if (_pollFds[i].fd == clientFd)
      {
        _pollFds[i].events |= POLLOUT;
        break;
      }
    }
  }
};