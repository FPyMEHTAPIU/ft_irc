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
		messageInfo msgInfo = {
			channel->getName(),
			client,
			nickname,
			client->getFd(),
			msg,
			true};
		channel->broadcast(server, msgInfo);
		std::string topic = channel->getTopic();
		if (!topic.empty())
		{
			msg += "332 " + nickname + " " + channelName + " :" + topic + "\r\n";
		}
		return msg;
	}
	catch (...)
	{
		std::shared_ptr<Channel> newChannel = std::make_shared<Channel>(channelName, client);
		server->addChannel(channelName, newChannel);
		return ":" + nickname + " JOIN " + channelName + "\r\n";
	}
}
