#include "../irc.hpp"

void handlePrivmsg(Server *server, std::vector<std::string> args, int senderId)
{
	try
	{
		auto senderPair = server->getClients().find(senderId);
		if (senderPair == server->getClients().end())
		{
			throw std::invalid_argument(":ircserv 401 PRIVMSG :No client found in" + std::to_string(senderId) +
										"socketFd \r\n");
		}

		Client &sender = senderPair->second;
		std::string senderNick = sender.getNick();

		if (args.size() != 3)
		{
			throw std::invalid_argument(":ircserv 461 " + senderNick +
										" PRIVMSG :Not enough parameters\r\n");
		}

		std::string target = args.at(1);
		std::string message = args.at(2);

		// check message
		if (message.empty())
		{
			throw std::invalid_argument(":ircserv 412 " + senderNick +
										" :No text to send\r\n");
		}

		// handle sending in channel
		if (!target.empty() && (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!'))
		{
			target = target.substr(1);
			auto channelPair = server->getChannels().find(target);
			if (channelPair == server->getChannels().end())
			{
				throw std::invalid_argument(":ircserv 403 PRIVMSG :No such channel " + senderNick + " " + target + " :No such channel\r\n");
			}
			std::shared_ptr<Channel> channel = channelPair->second;

			if (!channel->isMember(sender))
			{
				throw std::invalid_argument(":ircserv 404 " + senderNick + " " + target + " :Cannot send to channel\r\n");
			}

			// Broadcast to all members except sender
			for (auto &member : channel->getUsers())
			{
				int memberFd = member.getFd();
				if (memberFd != senderId)
				{
					std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + message + "\r\n";
					send(memberFd, out.c_str(), out.size(), 0);
					// member->enqueueMessage(out);
				}
			}
			return;
		}
		// handle sending in DM
		else
		{
			// TODO: FIX SENDER NICK DISAPPEARING
			std::cout << "1: " << target << " 2: " << senderNick << std::endl;
			Client &receiver = server->getClientByNick(target, senderNick);
			std::cout << "After\n";
			{
				std::cout << "Handling a DM\n";
				std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + message + "\r\n";
				send(receiver.getFd(), out.c_str(), out.size(), 0);
				// receiver->enqueueMessage(out);
			}
		}
	}
	catch (std::exception &e)
	{
		std::string err = e.what();
		// std::cerr << "Exception: " << err << std::endl;
		send(senderId, err.c_str(), err.size(), 0);
	}
}