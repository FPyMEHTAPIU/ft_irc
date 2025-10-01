#include "commands.hpp"

static void checkIfNickExists(Server *server, std::shared_ptr<Client> client, std::string &newNick)
{
	for (auto &c : server->getClients())
	{
		if (c.second->getNick() == newNick && c.second != client)
		{
			std::string clientNick = client->getNick().empty() ? "*" : client->getNick();
			client->enqueueMessage(":ircserv 433 " + clientNick + " " + newNick + " :Nickname is already in use\r\n");
			server->enableWrite(client->getFd());

			do
			{
				newNick += "_";
			} while (server->hasNick(newNick));
			break;
		}
	}
}

std::string handleNick(Server *server, std::shared_ptr<Client> client, std::string &newNick)
{
	if (newNick.empty())
		return ":ircserv 431 * :No nickname given\r\n";

	checkIfNickExists(server, client, newNick);

	std::string oldNick = client->getNick();
	client->setNick(newNick);

	std::string msg;

	if (oldNick.empty())
	{
		msg = ":" + newNick + " NICK :" + newNick + "\r\n";
	}
	else
	{
		msg = ":" + oldNick + "!" + client->getUsername() + "@localhost NICK :" + newNick + "\r\n";

		for (auto &ch : server->getChannels())
		{
			if (ch.second->hasUser(client))
				ch.second->broadcast(server, msg, client->getFd());
		}
	}

	return msg;
}
