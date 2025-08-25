#include "commands.hpp"

std::string handleJoin(Server *server, const std::vector<std::string> &args, const Client &client)
{
	std::string result = "";
	if (args.size() < 2)
	{
		result = "461 JOIN :Not enough parameters\r\n";
		return result;
	}

	auto it = std::find_if(
		server->getChannels().begin(),
		server->getChannels().end(),
		[&](const std::shared_ptr<Channel> &ch)
		{
			return ch->getName() == args[1];
		});

	if (it == server->getChannels().end())
	{
		// Create the channel and add the client
		std::shared_ptr<Channel> newChannel = std::make_shared<Channel>(
			args[1], client);
		server->addChannel(newChannel);
	}
	else
	{
		// The channel exists, add a new client
		(*it)->addUser(client);
	}

	result = ":" + client.getNick() + " JOIN " + args[1] + "\r\n";
	return result;
}
