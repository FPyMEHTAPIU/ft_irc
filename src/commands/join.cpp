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
	std::string nickname = client->getNick();
	if (!isValidChannelName(channelName))
	{
		return "476 " + nickname + " " + channelName + " :Bad Channel Mask\r\n";
	}

	try
	{
		std::shared_ptr<Channel> channel = server->getChannelByName(channelName);
		channel->addUser(client);

		std::string msg = ":" + nickname + " JOIN " + channelName + "\r\n";
		channel->broadcast(server, msg, -1);

		std::string topic = channel->getTopic();
		msg = "";
		if (!topic.empty())
		{
			msg += "332 " + nickname + " " + channelName + " :" + topic + "\r\n";
		}
		msg += channel->getNamesReply(nickname);
		return msg;
	}
	catch (...)
	{
		std::shared_ptr<Channel> newChannel = std::make_shared<Channel>(channelName, client);
		server->addChannel(channelName, newChannel);
		std::string msg = ":" + nickname + " JOIN " + channelName + "\r\n";
		msg += newChannel->getNamesReply(nickname);
		return msg;
	}
}
