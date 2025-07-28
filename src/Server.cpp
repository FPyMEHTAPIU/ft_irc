#include "Server.hpp"

Server::Server(int port, std::string password): _PORT(port), _PASSWORD(password) {}

Server::~Server() {}

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
