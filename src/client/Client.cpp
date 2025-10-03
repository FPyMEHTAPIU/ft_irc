#include "Client.hpp"
#include "../irc.hpp"

void Client::checkRegistration()
{
    _registered = (!_nick.empty() && !_username.empty());
}

Client::Client(int fd)
    : _fd(fd), _nick(""), _username(""), _realname(""), _registered(false), _authenticated(false) {}

Client::Client(int fd, const std::string &nick)
    : _fd(fd), _nick(nick), _username(""), _realname(""), _registered(false), _authenticated(false)
{
    checkRegistration();
}

Client::Client(int fd, const std::string &nick, const std::string &user, const std::string &real)
    : _fd(fd), _nick(nick), _username(user), _realname(real), _registered(false), _authenticated(false)
{
    checkRegistration();
}

Client::~Client() {}

bool Client::operator<(Client const &client) const
{
    return _fd < client._fd;
}

std::string Client::getNick() const { return _nick; }

std::string Client::getUsername() const { return _username; }

std::string Client::getRealName() const { return _realname; }

void Client::setNick(const std::string &newNick)
{
    _nick = newNick;
    checkRegistration();
}

void Client::setUser(const std::string &user, const std::string &real)
{
    _username = user;
    _realname = real;
    checkRegistration();
}

int Client::getFd() const { return _fd; }

void Client::enqueueMessage(const std::string &msg) { _writeQueue.push_back(msg); }

bool Client::hasPendingMessages() const { return !_writeQueue.empty(); }

std::string &Client::frontMessage() { return _writeQueue.front(); }

void Client::popMessage() { _writeQueue.pop_front(); }

bool Client::isRegistered() const { return _registered; }

void Client::authenticate() { _authenticated = true; }

bool Client::isAuthenticated() const { return _authenticated; }

std::string &Client::getBuffer() { return _buffer; }

void Client::clearBuffer() { _buffer.clear(); }
