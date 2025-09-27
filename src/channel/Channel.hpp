#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "../client/Client.hpp"
#include "../irc.hpp"

class Client;
class Server;
struct messageInfo;

class Channel
{
private:
  std::string _name;
  std::string _topic;
  std::string _key;
  std::vector<std::shared_ptr<Client>> _users;
  std::vector<std::shared_ptr<Client>> _operators;

  // Modes
  bool _isInviteOnly;
  bool _isTopicRestricted;
  // if 0 there's no limit
  size_t _userLimit;

public:
  Channel() = delete;
  Channel &operator=(Channel const &channel) = delete;
  Channel(std::string name, std::shared_ptr<Client> creator);
  ~Channel();
  bool operator<(std::shared_ptr<Channel> channel) const;

  std::vector<std::shared_ptr<Client>> getUsers();
  std::vector<std::shared_ptr<Client>> getOperators() const;
  std::string getName() const;
  std::string getTopic() const;
  bool getIsInviteOnly() const;
  bool isTopicRestricted() const;
  size_t getUserLimit() const;
  bool hasUser(std::shared_ptr<Client> client) const;
  bool isOperator(std::shared_ptr<Client> client) const;
  void broadcast(Server *server, const std::string &rawMessage, int excludeFd);
  std::string getNamesReply(const std::string &requesterNick) const;
  bool isEmpty() const;

  // Only for operators
  std::string getKey() const;
  void setKey(std::string newKey);
  void setName(std::string newName);
  void setTopic(std::string newName);
  void addUser(std::shared_ptr<Client> newUser);
  void addOperator(std::shared_ptr<Client> newOperator);
  void removeUser(std::shared_ptr<Client> oldUser);
  void removeOperator(std::shared_ptr<Client> oldOperator);
  void setIsInviteOnly(bool newMode);
  void setIsTopicRestricted(bool newMode);
  void setUserLimit(int newLimit);
};