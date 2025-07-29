#include "Channel.hpp"
#include "irc.hpp"

Channel::Channel(std::string name, Client creator): _name(name), _ID(generateRandomId()) {
  //_operators.insert(creator);
  (void)creator;
  _topic = "";
  _isInviteOnly = false;
  _isTopicChangeMode = false;
  _userLimit = 0;
}

Channel::~Channel() {}

bool Channel::operator<(Channel const& channel) const {
  return _name < channel._name;
}

// std::set<Client> Channel::getUsers() const {
//   return _users;
// }

// std::set<Client> Channel::getOperators() const {
//   return _operators;
// }

std::string Channel::getName() const {
  return _name;
}

std::string Channel::getId() const {
  return _ID;
}

std::string Channel::getTopic() const {
  return _topic;
}

bool Channel::getIsInviteOnly() const {
  return _isInviteOnly;
}

bool Channel::getIsTopicChangeMode() const {
  return _isTopicChangeMode;
}

size_t Channel::getUserLimit() const {
  return _userLimit;
}

void Channel::setName(std::string newName) {
  _name = newName;
}

void Channel::setTopic(std::string newTopic) {
  if (_isTopicChangeMode) {
    _topic = newTopic;
  }
  else {
    std::cout << "Topic is in readonly mode!" << std::endl;
  }
}

// void Channel::addUser(Client newUser) {
//   _users.insert(newUser);
// }

// void Channel::addOperator(Client newOperator) {
//   _operators.insert(newOperator);
// }

void Channel::setIsInviteOnly(bool newMode) {
  _isInviteOnly = newMode;
}

void Channel::setIsTopicChangeMode(bool newMode) {
  _isTopicChangeMode = newMode;
}

void Channel::setUserLimit(int newLimit) {
  _userLimit = newLimit;
}
