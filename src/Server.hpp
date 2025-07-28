#pragma once
#include <iostream>
#include <set>
#include "Channel.hpp"
#include "Client.hpp"

class Client;
class Channel;

class Server {
  private:
    const int _PORT;
    const std::string _PASSWORD;
    std::set<Channel> _channels;
    std::set<Client> _clients;

  public:
    Server() = delete;
    Server& operator=(Server const& server) = delete;
    Server(int port, std::string password);
    ~Server();

    std::set<Channel> getChannels() const;
    std::set<Client> getClients() const;
    void addChannel(Channel channel);
    void addClient(Client client);
};