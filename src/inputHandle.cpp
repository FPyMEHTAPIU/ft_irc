#include "irc.hpp"
#include "commands/commands.hpp"

std::string handleInput(const std::string &input, Server *server, int clientFd)
{
	if (input.empty())
		return "Empty command provided\r\n";

	std::vector<std::string> args = split(input, ' ');
	if (args.empty())
		return "";

	if (args[0].starts_with('/'))
	{
		args[0].erase(args[0].begin());
	}

	std::string cmdLowercase(args[0].size(), '\0');
	std::transform(args[0].begin(), args[0].end(),
								 cmdLowercase.begin(),
								 [](unsigned char c)
								 { return std::tolower(c); });

	validateCommand(cmdLowercase);

	Client &client = server->getClients().at(clientFd);
	std::string result = "";

	switch (hash(cmdLowercase.c_str(), cmdLowercase.size()))
	{
	case hash("nick"):
		if (args.size() < 2)
			result = "431 :No nickname given\r\n";
		else
			result = handleNick(client, args[1]);
		break;

	case hash("user"):
		if (args.size() < 5) // RFC requires 4 params
			result = "461 USER :Not enough parameters\r\n";
		else
			result = handleUser(client, args);
		break;

	case hash("join"):
		std::cout << "joining..." << args[1] << std::endl;
		result = handleJoin(server, args, client);
		break;

	case hash("cap"):
	{
		if (args.size() < 2)
		{
			result = "461 CAP :Not enough parameters\r\n";
		}
		else
		{
			std::string sub = args[1];
			std::transform(sub.begin(), sub.end(), sub.begin(), ::tolower);

			if (sub == "ls")
			{
				// Use * if client has no nickname yet
				std::string nick = client.getNick().empty() ? "*" : client.getNick();
				result = ":ircserv CAP " + nick + " LS :multi-prefix\r\n";
			}
			else if (sub == "req")
			{
				if (args.size() >= 3)
				{
					std::string capName = args[2];
					std::string nick = client.getNick().empty() ? "*" : client.getNick();
					result = ":ircserv CAP " + nick + " ACK :" + capName + "\r\n";
				}
			}
			else if (sub == "end")
			{
				result = "";
			}
		}
	}
	break;

	default:
		result = "421 " + args[0] + " :Unknown command\r\n";
		break;
	}

	return result;
}
