#include "commands.hpp"

static void handleChannelMessage(Server *server, messageInfo msgInfo)
{
	std::shared_ptr<Channel> channel = server->getChannelByName(msgInfo.target);

	if (!channel->isMember(msgInfo.sender))
	{
		throw std::invalid_argument(":ircserv 404 " + msgInfo.senderNick + " " + msgInfo.target + " :Cannot send to channel\r\n");
	}

	// Broadcast to all members except sender
	channel->broadcast(server, msgInfo);
	return;
}

static void handleDirectMessage(Server *server, messageInfo msgInfo)
{
	std::shared_ptr<Client> receiver = server->getClientByNick(msgInfo.target, msgInfo.senderNick);

	std::string out = ":" + msgInfo.senderNick + " PRIVMSG " + msgInfo.target + " :" + msgInfo.message + "\r\n";
	receiver->enqueueMessage(out);
	server->enableWrite(receiver->getFd());
}

void handlePrivmsg(Server *server, std::vector<std::string> args, int senderFd, const std::string &message)
{
	try
	{
		auto clients = server->getClients();
		auto senderPair = clients.find(senderFd);
		if (senderPair == clients.end())
		{
			throw std::invalid_argument(":ircserv 401 PRIVMSG :No client found in" + std::to_string(senderFd) +
										"socketFd \r\n");
		}

		std::shared_ptr<Client> sender = senderPair->second;
		std::string senderNick = sender->getNick();

		if (args.size() != 2)
		{
			throw std::invalid_argument(":ircserv 461 " + senderNick +
										" PRIVMSG :Not enough parameters\r\n");
		}

		std::string target = args.at(1);

		messageInfo msgInfo = {
			target,
			sender,
			senderNick,
			senderFd,
			message};

		// check message
		if (message.empty())
		{
			throw std::invalid_argument(":ircserv 412 " + senderNick +
										" :No text to send\r\n");
		}

		if (!target.empty() && (target.at(0) == '#' || target.at(0) == '&' || target.at(0) == '+' || target.at(0) == '!'))
		{
			handleChannelMessage(server, msgInfo);
		}
		else
		{
			handleDirectMessage(server, msgInfo);
		}
	}
	catch (std::exception &e)
	{
		std::string err = e.what();
		std::cerr << "Exception: " << err << std::endl;
	}
}