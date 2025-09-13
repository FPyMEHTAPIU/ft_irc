#include "commands.hpp"

std::string handleJoin(Server *server, const std::vector<std::string> &args, std::shared_ptr<Client> client)
{
	// Check registration
	if (!client->isRegistered())
		return "451 JOIN :You have not registered\r\n";

	// Require channel name
	if (args.size() < 2 || args.at(1).empty())
		return "461 JOIN :Not enough parameters\r\n";

	std::string channelName = args.at(1);
	if (!isValidChannelName(channelName))
	{
		return "476 " + client->getNick() + " " + channelName + " :Bad Channel Mask\r\n";
	}

	try
	{
		std::shared_ptr<Channel> channel = server->getChannelByName(channelName);
		channel->addUser(client);
	}
	catch (...)
	{
		std::shared_ptr<Channel> newChannel = std::make_shared<Channel>(channelName, client);
		server->addChannel(channelName, newChannel);
	}

	// Only send a proper JOIN message once
	return ":" + client->getNick() + " JOIN " + channelName + "\r\n";
}
