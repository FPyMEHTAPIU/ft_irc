#include "Channel.hpp"
#include "../irc.hpp"

Channel::Channel(std::string name, Client creator)
{

  if (name.empty())
    throw std::invalid_argument("Channel name cannot be empty");

  if (name[0] != '#')
  {
    name = "#" + name;
  }

  if (!isValidChannelName(name))
  {
    throw std::invalid_argument("Invalid channel name: " + name);
  }

  _name = name;
  _operators.push_back(creator);
  _topic = "";
  _isInviteOnly = false;
  _isTopicChangeMode = false;
  _userLimit = 0;
}

Channel::~Channel() {}

bool Channel::operator<(Channel const &channel) const
{
  return _name < channel._name;
}

std::vector<Client> Channel::getUsers() const
{
  return _users;
}

std::vector<Client> &Channel::getUsers()
{
  return _users;
}

std::vector<Client> Channel::getOperators() const
{
  return _operators;
}

std::string Channel::getName() const
{
  return _name;
}

std::string Channel::getTopic() const
{
  return _topic;
}

bool Channel::getIsInviteOnly() const
{
  return _isInviteOnly;
}

bool Channel::getIsTopicChangeMode() const
{
  return _isTopicChangeMode;
}

size_t Channel::getUserLimit() const
{
  return _userLimit;
}

bool Channel::isMember(const Client &client) const
{
  for (auto &user : _users)
  {
    if (user.getNick() == client.getNick())
    {
      return true;
    }
  }
  return false;
}

void Channel::setName(std::string newName)
{
  _name = newName;
}

void Channel::setTopic(std::string newTopic)
{
  if (_isTopicChangeMode)
  {
    _topic = newTopic;
  }
  else
  {
    std::cout << "Topic is in readonly mode!" << std::endl;
  }
}

void Channel::addUser(Client newUser)
{
  _users.push_back(newUser);
}

void Channel::addOperator(Client newOperator)
{
  _operators.push_back(newOperator);
}

void Channel::setIsInviteOnly(bool newMode)
{
  _isInviteOnly = newMode;
}

void Channel::setIsTopicChangeMode(bool newMode)
{
  _isTopicChangeMode = newMode;
}

void Channel::setUserLimit(int newLimit)
{
  _userLimit = newLimit;
}
