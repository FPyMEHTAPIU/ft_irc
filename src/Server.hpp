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

#define SERVER_BACKLOG 10 // max number of pending connections
#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK 2
#endif
// class Client;
class Channel;

class Server
{
private:
  const int _PORT;
  bool _status;
  const std::string _PASSWORD;

  std::set<std::shared_ptr<Channel>> _channels;
  std::map<int, Client> _clients;

  int _serverSocket;
  struct sockaddr_in _serverAddr;
  std::vector<struct pollfd> _pollFds;

  void setupSocket();
  void bindSocket();
  void listenSocket();

public:
  Server() = delete;
  Server &operator=(Server const &server) = delete;
  Server(int port, std::string password);
  ~Server();

  // Socket operations
  void start();
  void run();
  void stop();

  void acceptNewClient();
  void handleClientData(int clientFd);
  void handleClientWrite(int fd);
  void removeClient(int clientFd);

  std::set<std::shared_ptr<Channel>> getChannels() const;
  std::map<int, Client> getClients() const;
  std::vector<struct pollfd> getPollFds() const;
  void addChannel(std::shared_ptr<Channel> channel);
  void addClient(int fd, Client client);
};