#include "commands.hpp"

std::string handleNick(Server *server, std::shared_ptr<Client> client, const std::string &newNick)
{
	if (newNick.empty())
		return "431 :No nickname given\r\n";

	for (auto &c : server->getClients())
	{
		if (c.second->getNick() == newNick && c.second != client)
		{
			return "433 " + newNick + " :Nickname is already in use\r\n";
		}
	}

	std::string oldNick = client->getNick();

	if (oldNick.empty())
	{
		client->setNick(newNick);
		std::string prefix = generatePrefix(client);
		std::string result = prefix + " NICK registered\r\n";
		return result;
	}

	std::string prefix = generatePrefix(client);
	client->setNick(newNick);

	std::string msg = prefix + " NICK :" + newNick + "\r\n";
	for (auto &ch : server->getChannels())
	{
		if (ch.second->hasUser(client))
		{
			ch.second->broadcast(server, msg, client->getFd());
		}
	}

	return msg;
}
