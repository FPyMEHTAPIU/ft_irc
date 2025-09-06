#include "commands.hpp"

std::string handleJoin(Server *server, const std::vector<std::string> &args, Client &client)
{
	// Check registration
	if (!client.isRegistered())
		return "451 JOIN :You have not registered\r\n";

	// Require channel name
	if (args.size() < 2 || args[1].empty())
		return "461 JOIN :Not enough parameters\r\n";

	std::string channelName = args[1];
	if (!isValidChannelName(channelName))
	{
		return "476 " + client.getNick() + " " + channelName + " :Bad Channel Mask\r\n";
	}

	auto &channels = server->getChannels();
	auto channel = channels.find(channelName);

	if (channel == channels.end())
	{
		// Create the channel and add the client
		std::shared_ptr<Channel> newChannel = std::make_shared<Channel>(channelName, client);
		server->addChannel(channelName, newChannel);
	}
	else
	{
		channel->second->addUser(client); // <- access the shared_ptr via .second
	}

	// Only send a proper JOIN message once
	return ":" + client.getNick() + " JOIN " + channelName + "\r\n";
}
