#include "irc.hpp"
#include "commands/commands.hpp"

std::string handleInput(const std::string &input, Server *server, int clientFd)
{
	std::string result = "";

	if (input.empty())
	{
		result = "Empty command provided";
		return result;
	}

	std::vector<std::string> args = split(input, ' ');
	std::cout << "args: ";
	for (auto &arg : args)
	{
		std::cout << arg << " ";
	}
	std::cout << std::endl;

	if (args[0].starts_with('/'))
	{
		args[0].erase(args[0].begin());
	}
	// else
	// {
	// 	std::cout << "hui\n";
	// 	throw std::invalid_argument("The command must start from '/'");
	// }

	std::string cmdLowercase(args[0].size(), '\0');
	std::transform(args[0].begin(), args[0].end(),
				   cmdLowercase.begin(), ::tolower);

	validateCommand(cmdLowercase);

	Client &client = server->getClients().at(clientFd);

	switch (hash(cmdLowercase.c_str(), cmdLowercase.size()))
	{
	case hash("pass"):
		break;
	case hash("nick"):
		result = handleNick(client, args[1]);
		break;

	case hash("user"):
		result = handleUser(client, args);
		break;
	case hash("join"):
		result = handleJoin(server, args, client);
		break;
	case hash("part"):
		break;
	case hash("privmsg"):
		break;
	case hash("notice"):
		break;
	case hash("mode"):
		break;
	case hash("invite"):
		break;
	case hash("kick"):
		break;
	case hash("topic"):
		break;
	case hash("names"):
		break;
	case hash("list"):
		break;
	case hash("oper"):
		break;
	case hash("kill"):
		break;
	case hash("quit"):
		break;
	case hash("msg"):
		break;
	case hash("cap"):
	{
		if (args.size() < 2)
		{
			result = "461 CAP :Not enough parameters\r\n";
		}
		else
		{
			std::string secondArgLower;
			secondArgLower.resize(args[1].size());
			std::transform(args[1].begin(), args[1].end(), secondArgLower.begin(), ::tolower);

			if (secondArgLower == "ls")
			{
				result = ":ircserv CAP " + client.getNick() + " LS :multi-prefix\r\n";
			}
			else if (secondArgLower == "end")
			{
				result = ""; // ничего, регистрация может продолжиться
			}
		}
	}
	break;
	default:
		result = "Command not found" + input;
		std::cout << "default" << std::endl;
		break;
	}
	return result;
}