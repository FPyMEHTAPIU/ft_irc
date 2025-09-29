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

	std::string channelKeyAttempt = (args.size() >= 3 ? args[2] : "");

	try
	{
		std::shared_ptr<Channel> channel = server->getChannelByName(channelName);

		// Check if channel is invite-only and user is not invited
		if (channel->getIsInviteOnly() && !channel->isInvited(client))
		{
			return "473 " + nickname + " " + channelName + " :Cannot join channel (+i)\r\n";
		}

		// Check user limit
		if (channel->getUserLimit() > 0 && channel->getUsers().size() >= channel->getUserLimit())
		{
			return "471 " + nickname + " " + channelName + " :Cannot join channel (+l)\r\n";
		}

		// Remove from invite list if invited (invitation is consumed)
		if (channel->isInvited(client))
		{
			channel->removeInvite(client);
		}
		if (channel->isChannelKeyRequired() && !channel->validateChannelKey(channelKeyAttempt))
		{
			return "475 " + nickname + " " + channelName + " :Cannot join channel (+k)\r\n";
		}

		channel->addUser(client);

		std::string prefix = generatePrefix(client);
		std::string msg = prefix + " JOIN " + channelName + "\r\n";
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
		std::string prefix = generatePrefix(client);
		std::string msg = prefix + " JOIN " + channelName + "\r\n";
		msg += newChannel->getNamesReply(nickname);
		return msg;
	}
}
