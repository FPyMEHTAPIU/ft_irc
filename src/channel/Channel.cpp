#include "Channel.hpp"
#include "../irc.hpp"

Channel::Channel(std::string name, std::shared_ptr<Client> creator)
{

  if (name.empty())
    throw std::invalid_argument("Channel name cannot be empty");

  if (name.front() != '#')
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
  _isTopicRestricted = true;
  _userLimit = 0;
}

Channel::~Channel() {}

bool Channel::operator<(std::shared_ptr<Channel> channel) const
{
  return _name < channel->_name;
}

const std::vector<std::shared_ptr<Client>> &Channel::getUsers() const
{
  return _users;
}

const std::vector<std::shared_ptr<Client>> &Channel::getOperators() const
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

bool Channel::hasUser(std::shared_ptr<Client> client) const
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

bool Channel::isInvited(std::shared_ptr<Client> client) const
{
  for (auto &invitedUser : _inviteList)
  {
    if (invitedUser->getNick() == client->getNick())
    {
      return true;
    }
  }
  return false;
}

void Channel::broadcast(Server *server, const std::string &rawMessage, int excludeFd)
{
  for (auto &member : _users)
  {
    int memberFd = member->getFd();
    if (memberFd != excludeFd)
    {
      member->enqueueMessage(rawMessage);
      server->enableWrite(memberFd);
    }
  }
}

std::string Channel::getNamesReply(const std::string &requesterNick) const
{
  std::string names;
  for (auto &user : _users)
  {
    if (isOperator(user))
      names += "@" + user->getNick() + " ";
    else
      names += user->getNick() + " ";
  }
  if (!names.empty())
    names.pop_back();

  return ":ircserv 353 " + requesterNick + " = " + _name + " :" + names + "\r\n" +
         ":ircserv 366 " + requesterNick + " " + _name + " :End of /NAMES list\r\n";
}

bool Channel::isEmpty() const
{
  return _users.size() == 0;
}

bool Channel::isChannelKeyRequired() const
{
  return !_key.empty();
}

bool Channel::validateChannelKey(const std::string &attempt) const
{
  if (!isChannelKeyRequired())
    return true; // no key required
  return attempt == _key;
}

std::string Channel::getChannelKey() const
{
  return _key;
}

void Channel::setChannelKey(std::string newKey)
{
  _key = newKey;
}

void Channel::setName(std::string newName)
{
  _name = newName;
}

void Channel::setTopic(std::string newTopic)
{
  _topic = newTopic;
}

void Channel::addUser(std::shared_ptr<Client> newUser)
{
  _users.push_back(newUser);
}

void Channel::addOperator(std::shared_ptr<Client> newOperator)
{
  _operators.push_back(newOperator);
}

void Channel::removeUser(std::shared_ptr<Client> oldUser)
{
  auto it = std::find_if(
      _users.begin(),
      _users.end(),
      [&](const std::shared_ptr<Client> &user)
      {
        return user->getFd() == oldUser->getFd();
      });

  if (it != _users.end())
  {
    _users.erase(it);
    removeOperator(oldUser);
  }
}

void Channel::removeOperator(std::shared_ptr<Client> oldOperator)
{
  auto it = std::find_if(
      _operators.begin(),
      _operators.end(),
      [&](const std::shared_ptr<Client> &op)
      {
        return op->getFd() == oldOperator->getFd();
      });

  if (it != _operators.end())
    _operators.erase(it);
}

void Channel::addInvite(std::shared_ptr<Client> client)
{
  // Don't invite if already a member or already invited
  if (!hasUser(client) && !isInvited(client))
  {
    _inviteList.push_back(client);
  }
}

void Channel::removeInvite(std::shared_ptr<Client> client)
{
  auto it = std::find_if(
      _inviteList.begin(),
      _inviteList.end(),
      [&](const std::shared_ptr<Client> &invitedUser)
      {
        return invitedUser->getFd() == client->getFd();
      });

  if (it != _inviteList.end())
    _inviteList.erase(it);
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