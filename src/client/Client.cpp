#include "Client.hpp"
#include "../irc.hpp"

Client::Client(std::string name) : _nickname(name) {}

Client::~Client() {}

bool Client::operator<(Client const &client) const
{
	return _nickname < client._nickname;
}

std::string Client::getName() const
{
	return _nickname;
}

void Client::setName(std::string newName)
{
	_nickname = newName;
}
