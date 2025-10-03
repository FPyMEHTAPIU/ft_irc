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

  void checkRegistration();

public:
  Client(int fd);
  Client(int fd, const std::string &nick);
  Client(int fd, const std::string &nick, const std::string &user, const std::string &real);

  Client &operator=(Client const &client) = delete;

  ~Client();

  bool operator<(Client const &client) const;

  std::string getNick() const;
  std::string getUsername() const;
  std::string getRealName() const;
  void setNick(const std::string &newNick);
  void setUser(const std::string &user, const std::string &real);

  int getFd() const;

  // Message queue helpers
  void enqueueMessage(const std::string &msg);
  bool hasPendingMessages() const;
  std::string &frontMessage();
  void popMessage();

  bool isRegistered() const;

  // Authentication helpers
  void authenticate();
  bool isAuthenticated() const;

  std::string &getBuffer();
  void clearBuffer();
};
