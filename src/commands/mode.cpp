#include "commands.hpp"

bool isIgnoredFlag(const std::string &arg)
{
	for (char c : arg)
	{
		if (c == '+')
			continue;
		if (c == 'b' || c == 'e')
			return true;
	}
	return false;
}

std::string handleMode(Server *server, const std::vector<std::string> &args,
					   std::shared_ptr<Client> client)
{
	if (args.size() < 2)
		return "461 MODE :Not enough parameters\r\n";

	std::string channelName = args.at(1);
	std::shared_ptr<Channel> channel;
	std::string nickname = client->getNick();

	if (channelName.find_first_of("#!&+", 0, 1) == std::string::npos)
		return "";

	try
	{
		channel = server->getChannelByName(channelName);
	}
	catch (...)
	{
		return "403 " + nickname + " " + channelName + " :No such channel\r\n";
	}

	if (args.size() == 2)
	{
		std::string modes = "+";
		if (channel->getIsInviteOnly())
			modes += "i";
		if (channel->isTopicRestricted())
			modes += "t";
		if (!channel->getChannelKey().empty())
			modes += "k";
		if (channel->getUserLimit() > 0)
			modes += "l";
		return "324 " + nickname + " " + channelName + " " + modes + "\r\n";
	}

	std::string modeStr = args.at(2);
	if (isIgnoredFlag(modeStr))
		return "";

	// Require operator
	if (!channel->isOperator(client))
	{
		return "482 " + nickname + " " + channelName + " :You're not channel operator\r\n";
	}

	// MODE changes

	bool adding = true;
	size_t argIndex = 3; // arguments after mode string

	for (char c : modeStr)
	{
		if (c == '+')
		{
			adding = true;
			continue;
		}
		if (c == '-')
		{
			adding = false;
			continue;
		}

		switch (c)
		{
		case 'i':
			channel->setIsInviteOnly(adding);
			break;

		case 't':
			channel->setIsTopicRestricted(adding);
			break;

		case 'k':
			if (adding)
			{
				if (argIndex >= args.size())
					return "461 MODE :Not enough parameters\r\n";
				channel->setChannelKey(args.at(argIndex++));
			}
			else
			{
				channel->setChannelKey("");
			}
			break;

		case 'o':
			if (argIndex >= args.size())
				return "461 MODE :Not enough parameters\r\n";
			{
				std::string targetNick = args.at(argIndex++);
				std::shared_ptr<Client> target = server->getClientByNick(targetNick, nickname);
				if (!target)
				{
					return "401 " + nickname + " " + targetNick + " :No such nick\r\n";
				}
				if (!channel->hasUser(target))
				{
					return "";
				}
				if (adding)
					channel->addOperator(target);
				else
					channel->removeOperator(target);
			}
			break;

		case 'l':
			if (adding)
			{
				if (argIndex >= args.size())
					return "461 MODE :Not enough parameters\r\n";
				size_t limit = std::stoul(args.at(argIndex++));
				channel->setUserLimit(limit);
			}
			else
			{
				channel->setUserLimit(0);
			}
			break;

		default:
			return "472 " + nickname + " " + c + " :is unknown mode char\r\n";
		}
	}

	std::string prefix = generatePrefix(client);
	std::string msg = prefix + " MODE " + channelName + " " + modeStr;
	for (size_t i = 3; i < args.size(); i++)
		msg += " " + args.at(i);
	msg += "\r\n";

	channel->broadcast(server, msg, -1);
	return "";
}
