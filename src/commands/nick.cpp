#include "commands.hpp"

std::string handleNick(std::shared_ptr<Client> client, const std::string &nick)
{
	client->setNick(nick);
	std::string prefix = generatePrefix(client);
	std::string result = prefix + " NICK registered\r\n";
	return result;
}
