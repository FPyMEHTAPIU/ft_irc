#include "irc.hpp"
#include "commands/commands.hpp"
#include "Server.hpp"

void handleInput(std::string input, Server *server, int clientFd)
{
	std::string result = "";
	try
	{
		// remove garbage return char
		input.pop_back();
		std::vector<std::string> args;
		std::vector<std::string> splittedMsg = split(input, ':');
		std::string msg = "";

		if (splittedMsg.size() > 1)
		{
			std::cout << "Debug: " << splittedMsg.at(0) << std::endl;
			args = split(splittedMsg.at(0), ' ');
			msg = splittedMsg.at(1);
			std::cout << "msg: " << splittedMsg.at(1) << std::endl;
		}
		else
		{
			args = split(input, ' ');
		}

		if (args.empty())
			return;

		if (startsWith(args[0], "/"))
		{
			args.at(0).erase(args.at(0).begin());
		}

		std::string cmdLowercase = strToLowercase(args.at(0));

		validateCommand(cmdLowercase);

		Client &client = server->getClients().at(clientFd);

		switch (hash(cmdLowercase.c_str(), cmdLowercase.size()))
		{
		case hash("nick"):
			if (args.size() < 2)
				result = "431 :No nickname given\r\n";
			else
				result = handleNick(client, args.at(1));
			break;

		case hash("user"):
			if (args.size() < 4) // RFC requires 4 params
				result = "461 USER :Not enough parameters\r\n";
			else
				result = handleUser(client, args, msg);
			break;

		case hash("join"):
			std::cout << "joining..." << args.at(1) << std::endl;
			result = handleJoin(server, args, client);
			break;

		case hash("privmsg"):
			handlePrivmsg(server, args, clientFd, msg);
			return;
		case hash("pass"):
			if (args.size() < 2)
				result = "461 PASS :Not enough parameters\r\n";
			try
			{
				validatePassword(args[1]);
			}
			catch (const std::exception &e)
			{
				result = std::string("464 PASS :") + e.what() + "\r\n";
			}
			if (args[1] != server->getPassword())
				result = "464 PASS :Password incorrect\r\n";

			// server->getClients()[clientFd].setAuthenticated(true); //when authentication has been set up
			result = "Password accepted\r\n";
			break;

		default:
			break;
		}
		client.enqueueMessage(result);
		server->enableWrite(clientFd);
		// send(clientFd, result.c_str(), result.length(), 0);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
