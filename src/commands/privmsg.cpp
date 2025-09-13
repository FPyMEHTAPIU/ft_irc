#include "../irc.hpp"

void handlePrivmsg(Server *server, std::vector<std::string> args, int senderId, std::string msg)
{
	try
	{
		auto senderPair = server->getClients().find(senderId);
		if (senderPair == server->getClients().end())
		{
			throw std::invalid_argument(":ircserv 401 PRIVMSG :No client found in" + std::to_string(senderId) +
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
		std::string message = msg;

		// check message
		if (message.empty())
		{
			throw std::invalid_argument(":ircserv 412 " + senderNick +
										" :No text to send\r\n");
		}

		// handle sending in channel
		if (!target.empty() && (target.at(0) == '#' || target.at(0) == '&' || target.at(0) == '+' || target.at(0) == '!'))
		{
			std::shared_ptr<Channel> channel = server->getChannelByName(target);

			if (!channel->isMember(sender))
			{
				throw std::invalid_argument(":ircserv 404 " + senderNick + " " + target + " :Cannot send to channel\r\n");
			}

			// Broadcast to all members except sender
			for (auto member : channel->getUsers())
			{
				int memberFd = member->getFd();
				if (memberFd != senderId)
				{
					std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + message + "\r\n";
					member->enqueueMessage(out);
					server->enableWrite(memberFd);
				}
			}
			return;
		}
		// handle sending in DM
		else
		{
			std::shared_ptr<Client> receiver = server->getClientByNick(target, senderNick);
			{
				std::cout << "Handling a DM, sender: " << senderNick << " receiver: " << receiver->getNick() << " target: " << target << std::endl;
				// std::string out = ":" + senderNick + "!" + sender.getUsername() + "@ircserv PRIVMSG " + target + " :" + message + "\r\n";
				std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + message + "\r\n";
				receiver->enqueueMessage(out);
				server->enableWrite(receiver->getFd());
			}
		}
	}
	catch (std::exception &e)
	{
		std::string err = e.what();
		std::cerr << "Exception: " << err << std::endl;
		// send(senderId, err.c_str(), err.size(), 0);
	}
}