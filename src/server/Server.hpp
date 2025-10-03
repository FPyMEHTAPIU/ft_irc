#pragma once
#include <map>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../irc.hpp"
#include "../common/socket_utils.hpp"

class Channel;
class Client;
class Logger;

class Server
{
private:
  const int _PORT;
  const std::string _PASSWORD;
  static volatile bool _terminate;

  int _serverSocket;

  std::map<std::string, std::shared_ptr<Channel>> _channels;
  std::map<int, std::shared_ptr<Client>> _clients;

  struct sockaddr_in _serverAddr;
  std::vector<struct pollfd> _pollFds;

  void setupSocket();
  void bindSocket();
  void listenSocket();

public:
  std::shared_ptr<Logger> logger;

  Server() = delete;
  Server &operator=(Server const &server) = delete;
  Server(int port, std::string password, std::shared_ptr<Logger> logger);
  ~Server();

  void setupSignals(bool serverStart) noexcept;
  static void signalHandler(int signum);

  void start();
  void run();
  void stop();

  void acceptNewClient();
  void handleClientData(int clientFd);
  void handleClientWrite(int fd);
  void removeClient(int clientFd);
  void removeChannel(std::string channelName);

  // Server getters
  const std::map<std::string, std::shared_ptr<Channel>> &getChannels() const;
  const std::map<int, std::shared_ptr<Client>> &getClients() const;
  const std::vector<struct pollfd> &getPollFds() const;
  const std::string &getPassword() const;
  std::shared_ptr<Client> getClientByNick(const std::string &nick, const std::string &senderNick);
  std::shared_ptr<Client> getClientByFd(const int &fd);
  std::shared_ptr<Channel> getChannelByName(const std::string &channelName);
  bool hasNick(const std::string &nick) const;

  // Safe client lookup methods
  std::shared_ptr<Client> findClientByNick(const std::string &nick) const;
  std::shared_ptr<Client> findClientByFd(const int &fd) const;

  void addChannel(const std::string &channelName, std::shared_ptr<Channel> channel);
  void addClient(int fd, Client client);

  void enableWrite(int clientFd);
  void disableWrite(int fd);
};