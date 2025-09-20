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
  // bool _status;
  const std::string _PASSWORD;
  static volatile bool _terminate;

  int _serverSocket;
  Logger *_logger;

  std::map<std::string, std::shared_ptr<Channel>> _channels;
  std::map<int, std::shared_ptr<Client>> _clients;

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

  void signalSetup(bool start) noexcept;
  static void signalHandler(int signum);

  void start();
  void run();
  void stop();

  void acceptNewClient();
  void handleClientData(int clientFd);
  void handleClientWrite(int fd);
  void removeClient(int clientFd);

  // Server getters
  std::map<std::string, std::shared_ptr<Channel>> getChannels();
  std::map<int, std::shared_ptr<Client>> getClients();
  std::vector<struct pollfd> getPollFds() const;
  const std::string &getPassword() const;
  std::shared_ptr<Client> getClientByNick(const std::string &nick, const std::string &senderNick);
  std::shared_ptr<Channel> getChannelByName(const std::string &channelName);

  void addChannel(const std::string &channelName, std::shared_ptr<Channel> channel);
  void addClient(int fd, Client client);

  void enableWrite(int clientFd)
  {
    for (size_t i = 0; i < _pollFds.size(); ++i)
    {
      if (_pollFds.at(i).fd == clientFd)
      {
        _pollFds.at(i).events |= POLLOUT;
        break;
      }
    }
  }

  void disableWrite(int fd)
  {
    for (size_t i = 0; i < _pollFds.size(); ++i)
    {
      if (_pollFds[i].fd == fd)
      {
        _pollFds[i].events &= ~POLLOUT;
        return;
      }
    }
  }
};