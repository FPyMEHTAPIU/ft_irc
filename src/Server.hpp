#pragma once
#include <iostream>
#include <set>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Channel.hpp"
#include "Client.hpp"

#define SERVER_BACKLOG 10 // max number of pending connections
#ifndef SOCK_NONBLOCK
# define SOCK_NONBLOCK 2
#endif
// class Client;
class Channel;

class Server
{
private:
  const int _PORT;
  bool _status;
  const std::string _PASSWORD;

  std::set<Channel> _channels;
  std::set<Client> _clients;

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
  void removeClient(int clientFd);

  std::set<Channel> getChannels() const;
  std::set<Client> getClients() const;
  void addChannel(Channel channel);
  void addClient(Client client);
};