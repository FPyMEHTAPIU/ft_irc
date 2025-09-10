#pragma once
#include <iostream>
#include <deque>
#include <string>

class Client
{
private:
  int _fd;
  std::string _nick;
  std::string _username;
  std::string _realname;
  std::deque<std::string> _writeQueue;
  bool registered;

  void checkRegistration()
  {
    registered = (!_nick.empty() && !_username.empty());
  }

public:
  Client(int fd)
      : _fd(fd), _nick(""), _username(""), _realname(""), registered(false) {}

  Client(int fd, const std::string &nick)
      : _fd(fd), _nick(nick), _username(""), _realname(""), registered(false)
  {
    checkRegistration();
  }

  Client(int fd, const std::string &nick, const std::string &user, const std::string &real)
      : _fd(fd), _nick(nick), _username(user), _realname(real), registered(false)
  {
    checkRegistration();
  }

  Client &operator=(Client const &client) = delete;

  ~Client() {}

  bool operator<(Client const &client) const
  {
    return _fd < client._fd;
  }

  std::string getNick() const { return _nick; }
  void setNick(const std::string &newNick)
  {
    _nick = newNick;
    checkRegistration();
  }

  void setUser(const std::string &user, const std::string &real)
  {
    _username = user;
    _realname = real;
    checkRegistration();
  }

  int getFd() const { return _fd; }

  void enqueueMessage(const std::string &msg) { _writeQueue.push_back(msg); }
  bool hasPendingMessages() const { return !_writeQueue.empty(); }
  std::string &frontMessage() { return _writeQueue.front(); }
  void popMessage() { _writeQueue.pop_front(); }

  bool isRegistered() const { return registered; }
};
