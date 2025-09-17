#include "irc.hpp"
#include "commands/commands.hpp"
#include "Server.hpp"

std::vector<std::string> trimSplitInput(std::string &input, std::string &msg)
{
	// remove cariage return char
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
				result = handleNick(client, args.at(1));
			break;

		case hash("user"):
			if (args.size() < 4) // RFC requires 4 params
				result = "461 USER :Not enough parameters\r\n";
			else
				result = handleUser(client, args, msg);
			break;

		case hash("join"):
			result = handleJoin(server, args, client);
			break;

		case hash("privmsg"):
			handlePrivmsg(server, args, clientFd, msg);
			return;

		case hash("pass"):
			std::cout << "[DEBUG] Received PASS command with password: " << args[1] << std::endl; // Log the received password

			if (args.size() < 2)
			{
				result = "461 PASS :Not enough parameters\r\n";
			}
			else
			{
				try
				{
					// Debugging the password validation step
					std::cout << "[DEBUG] Validating password..." << std::endl;
					validatePasswordNew(args[1], storedPassword);
					result = "Password accepted\r\n";
					std::cout << "[DEBUG] Password accepted!" << std::endl;
				}
				catch (const std::exception &e)
				{
					std::cout << "[DEBUG] Password validation failed: " << e.what() << std::endl; // Log the error
					result = std::string("464 PASS :") + e.what() + "\r\n";
				}
			}
			break;
		case hash("ping"):
			handlePing(args);
			return;
		default:
			break;
		}
		client->enqueueMessage(result);
		server->enableWrite(clientFd);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
