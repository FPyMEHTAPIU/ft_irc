#include "commands.hpp"

std::string handleNick(std::shared_ptr<Client> client, const std::string &nick)
{
	client->setNick(nick);
	std::string result = ":" + client->getNick() + " NICK registered\r\n";
	std::cout << "Handling nick: " << result << std::endl;
	return result;
}
