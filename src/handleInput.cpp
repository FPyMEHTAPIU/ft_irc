#include "irc.hpp"
#include "commands/commands.hpp"
#include "Server.hpp"

std::vector<std::string> trimSplitInput(std::string &input, std::string &msg)
{
	// remove carriage return characters
	input.pop_back();
	std::vector<std::string> args;
	std::vector<std::string> splittedMsg = split(input, ':');

	if (splittedMsg.size() > 1)
	{
		args = split(splittedMsg.at(0), ' ');
		msg = splittedMsg.at(1);
	}
	else
	{
		args = split(input, ' ');
	}

	if (args.empty())
		return args;

	if (args.at(0).starts_with('/'))
	{
		args.at(0).erase(args.at(0).begin());
	}
	return args;
}

void handleInput(std::string input, Server *server, int clientFd)
{
	std::string result = "";
	try
	{
		std::string msg = "";
		std::vector<std::string> args = trimSplitInput(input, msg);
		if (args.empty())
			return;

		std::string cmdLowercase = strToLowercase(args.at(0));

		validateCommand(cmdLowercase);

		std::shared_ptr<Client> client = server->getClients().at(clientFd);

		std::string storedPassword = server->getPassword();

		switch (hash(cmdLowercase.c_str(), cmdLowercase.size()))
		{
		case hash("nick"):
			if (args.size() < 2)
				result = "431 :No nickname given\r\n";
			else
				result = handleNick(server, client, args.at(1));
			break;

		case hash("user"):
			if (args.size() < 4)
				result = "461 USER :Not enough parameters\r\n";
			else
				result = handleUser(client, args, msg);
			break;

		case hash("join"):
			result = handleJoin(server, args, client);
			break;
		case hash("invite"):
			std::cout << "inviting..." << args.at(1) << std::endl;
			result = handleInvite(server, args, client);
			break;
		case hash("leave"):
		case hash("part"):
			std::cout << "leaving channel..." << args.at(1) << std::endl;
			result = handlePart(server, args, client, msg);
			break;

		case hash("privmsg"):
			handlePrivmsg(server, args, clientFd, msg);
			return;

		case hash("pass"):
			if (args.size() < 2)
			{
				result = "461 PASS :Not enough parameters\r\n";
			}
			else
			{
				try
				{
					server->logger->debug(AUTH, "Validating password...");
					validateClientPassword(args[1], storedPassword);
					result = "Password accepted\r\n";
					server->logger->info(AUTH, "Authenticated " + client->getNick() + "!");
				}
				catch (const std::exception &e)
				{
					server->logger->error(AUTH, "Password validation failed");
					result = std::string("464 PASS :") + e.what() + "\r\n";
				}
			}
			break;
		case hash("ping"):
			result = handlePing(args);
			break;
		case hash("topic"):
			result = handleTopic(server, args, msg, client);
			break;
		case hash("mode"):
			result = handleMode(server, args, client);
			break;
		case hash("who"):
			result = handleWho(server, args, client);
			break;
		case hash("kick"):
			result = handleKick(server, args, client, msg);
			break;
		default:
			break;
		}
		if (result.empty())
			return;
		client->enqueueMessage(result);
		server->enableWrite(clientFd);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
