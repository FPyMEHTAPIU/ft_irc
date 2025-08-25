#include "commands.hpp"

std::string handleNick(Client &client, const std::string &nick)
{
	client.setNick(nick);
	std::string result = ":" + client.getNick() + " NICK registered\r\n";
	return result;
}
