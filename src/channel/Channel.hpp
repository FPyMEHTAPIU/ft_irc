#pragma once
#include <iostream>
#include <set>
#include "../client/Client.hpp"

class Client;

class Channel
{
private:
  std::string _name;
  std::string _topic;
  std::set<Client> _users;
  std::set<Client> _operators;

  // Modes
  bool _isInviteOnly;
  bool _isTopicChangeMode;
  // if 0 there's no limit
  size_t _userLimit;

public:
  Channel() = delete;
  Channel &operator=(Channel const &channel) = delete;
  Channel(std::string name, Client creator);
  ~Channel();
  bool operator<(Channel const &channel) const;

  std::set<Client> getUsers() const;
  std::set<Client> getOperators() const;
  std::string getName() const;
  std::string getTopic() const;
  bool getIsInviteOnly() const;
  bool getIsTopicChangeMode() const;
  size_t getUserLimit() const;

  // Only for operators
  void setName(std::string newName);
  void setTopic(std::string newName);
  void addUser(Client newUser);
  void addOperator(Client newOperator);
  void setIsInviteOnly(bool newMode);
  void setIsTopicChangeMode(bool newMode);
  void setUserLimit(int newLimit);
};