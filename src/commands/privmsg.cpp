#include "../irc.hpp"

void handlePrivmsg(Server *server, std::vector<std::string> args, int senderId)
{
	(void)server;
	(void)senderId;

	try
	{
		auto senderPair = server->getClients().find(senderId);
		if (senderPair == server->getClients().end())
		{
			throw new std::invalid_argument(":ircserv 401 PRIVMSG :No client found in" + std::to_string(senderId) +
											"socketFd \r\n");
		}

		Client sender = senderPair->second;
		std::string senderNick = sender.getNick();

		if (args.size() != 3)
		{
			throw new std::invalid_argument(":ircserv 461 " + senderNick +
											" PRIVMSG :Not enough parameters\r\n");
		}

		std::string target = args.at(1);
		std::string message = args.at(2);

		// check message
		if (message.empty())
		{
			throw new std::invalid_argument(":ircserv 412 " + senderNick +
											" :No text to send\r\n");
		}

		// handle sending in channel
		if (!target.empty() && (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!'))
		{
			target = target.substr(1);
			auto channelPair = server->getChannels().find(target);
			if (channelPair == server->getChannels().end())
			{
				throw new std::invalid_argument(":ircserv 403 PRIVMSG :No such channel " + senderNick + " " + target + " :No such channel\r\n");
			}
			std::shared_ptr<Channel> channel = channelPair->second;

			if (!channel->isMember(sender))
			{
				throw new std::invalid_argument(":ircserv 404 " + senderNick + " " + target + " :Cannot send to channel\r\n");
			}

			// Broadcast to all members except sender
			for (auto &member : channel->getUsers())
			{
				int memberFd = member.getFd();
				if (memberFd != senderId)
				{
					std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + message + "\r\n";
					send(memberFd, message.c_str(), message.length(), 0);
					// member->enqueueMessage(out);
				}
			}
			return;
		}

		// handle sending in DM
		Client receiver = server->getClientByNick(target, senderNick); // implement findClientByNick()

		// Deliver to target user
		{
			std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + message + "\r\n";
			send(receiver.getFd(), message.c_str(), message.length(), 0);
			// receiver->enqueueMessage(out);
		}
	}
	catch (std::exception &e)
	{
		send(senderId, e.what(), sizeof(e.what()), 0);
	}

	// check num of args - DONE
	// check next arg - DONE
	// get all client in a channel or choose only one
	// send data to all of them
}