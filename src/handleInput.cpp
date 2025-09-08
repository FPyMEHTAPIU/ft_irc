#include "irc.hpp"
#include "commands/commands.hpp"
#include "Server.hpp"

std::string handleInput(const std::string &input, Server *server, int clientFd)
{
	std::string result = "";
	try
	{
		std::vector<std::string> args = split(input, ' ');
		if (args.empty())
			return "";

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
			if (args.size() < 5) // RFC requires 4 params
				result = "461 USER :Not enough parameters\r\n";
			else
				result = handleUser(client, args);
			break;

		case hash("join"):
			std::cout << "joining..." << args.at(1) << std::endl;
			result = handleJoin(server, args, client);
			break;

		case hash("pass"):
			if (args.size() < 2)
				return "461 PASS :Not enough parameters\r\n";
			try
			{
				validatePassword(args[1]);
			}
			catch (const std::exception &e)
			{
				return std::string("464 PASS :") + e.what() + "\r\n";
			}
			if (args[1] != server->getPassword())
				result = "464 PASS :Password incorrect\r\n";

			// server->getClients()[clientFd].setAuthenticated(true); //when authentication has been set up
			return "Password accepted\r\n";
			break;

		default:
			break;
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return result;
}
