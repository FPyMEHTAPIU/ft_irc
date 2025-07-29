#include "Client.hpp"
#include "irc.hpp"

Client::Client(std::string name): _name(name), _ID(generateRandomId()) {}

Client::~Client() {}

bool Client::operator<(Client const& client) const {
	return _name < client._name;
}

std::string Client::getName() const {
	return _name;
}

std::string Client::getId() const {
	return _ID;
}

void Client::setName(std::string newName) {
	_name = newName;
}
