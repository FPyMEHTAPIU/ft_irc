#include "Channel.hpp"
#include "../irc.hpp"

Channel::Channel(std::string name, std::shared_ptr<Client> creator)
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
  _users.push_back(creator);
  _topic = "";
  _isInviteOnly = false;
  _isTopicRestricted = false;
  _userLimit = 0;
}

Channel::~Channel() {}

bool Channel::operator<(std::shared_ptr<Channel> channel) const
{
  return _name < channel->_name;
}

std::vector<std::shared_ptr<Client>> Channel::getUsers()
{
  return _users;
}

std::vector<std::shared_ptr<Client>> Channel::getOperators() const
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

bool Channel::isTopicRestricted() const
{
  return _isTopicRestricted;
}

size_t Channel::getUserLimit() const
{
  return _userLimit;
}

bool Channel::isMember(std::shared_ptr<Client> client) const
{
  for (auto &user : _users)
  {
    if (user->getNick() == client->getNick())
    {
      return true;
    }
  }
  return false;
}

bool Channel::isOperator(std::shared_ptr<Client> client) const
{
  for (auto &oper : _operators)
  {
    if (oper->getNick() == client->getNick())
    {
      return true;
    }
  }
  return false;
}

void Channel::broadcast(Server *server, messageInfo msgInfo)
{
  for (auto member : _users)
  {
    int memberFd = member->getFd();
    if (memberFd != msgInfo.senderFd)
    {
      std::string out = ":" + msgInfo.senderNick + " PRIVMSG " + msgInfo.target + " :" + msgInfo.message + "\r\n";
      member->enqueueMessage(out);
      server->enableWrite(memberFd);
    }
  }
}

void Channel::setName(std::string newName)
{
  _name = newName;
}

void Channel::setTopic(std::string newTopic)
{
  if (!_isTopicRestricted)
  {
    _topic = newTopic;
  }
  else
  {
    std::cout << "Topic is in readonly mode!" << std::endl;
  }
}

void Channel::addUser(std::shared_ptr<Client> newUser)
{
  _users.push_back(newUser);
}

void Channel::addOperator(std::shared_ptr<Client> newOperator)
{
  _operators.push_back(newOperator);
}

void Channel::setIsInviteOnly(bool newMode)
{
  _isInviteOnly = newMode;
}

void Channel::setIsTopicRestricted(bool newMode)
{
  _isTopicRestricted = newMode;
}

void Channel::setUserLimit(int newLimit)
{
  _userLimit = newLimit;
}
