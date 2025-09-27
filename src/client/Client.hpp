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
  bool _registered;
  std::string _buffer;
  bool _authenticated;

  void checkRegistration()
  {
    _registered = (!_nick.empty() && !_username.empty());
  }

public:
  Client(int fd)
      : _fd(fd), _nick(""), _username(""), _realname(""), _registered(false), _authenticated(false) {}

  Client(int fd, const std::string &nick)
      : _fd(fd), _nick(nick), _username(""), _realname(""), _registered(false)
  {
    checkRegistration();
  }

  Client(int fd, const std::string &nick, const std::string &user, const std::string &real)
      : _fd(fd), _nick(nick), _username(user), _realname(real), _registered(false)
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
  std::string getUsername() const { return _username; }
  std::string getRealName() const { return _realname; }
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

  // Message queue helpers
  void enqueueMessage(const std::string &msg) { _writeQueue.push_back(msg); }
  bool hasPendingMessages() const { return !_writeQueue.empty(); }
  std::string &frontMessage() { return _writeQueue.front(); }
  void popMessage() { _writeQueue.pop_front(); }

  bool isRegistered() const { return _registered; };

  // Authentication helpers
  void authenticate() { _authenticated = true; };
  bool isAuthenticated() const { return _authenticated; };

  std::string &getBuffer() { return _buffer; }
  void clearBuffer() { _buffer.clear(); }
};
