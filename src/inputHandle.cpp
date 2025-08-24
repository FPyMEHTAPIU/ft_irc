#include "irc.hpp"

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
		client.setNick(args[1]);
		result = ":" + client.getNick() + " NICK registered\r\n";
		break;

	case hash("user"):
		client.setUser(args[1], args[4].substr(1)); // отбрасываем ':'
		if (client.isRegistered())
		{
			result = ":ircserv 001 " + client.getNick() + " :Welcome to the IRC network\r\n";
			result += ":ircserv 002 " + client.getNick() + " :Your host is ircserv\r\n";
		}
		break;
	case hash("join"):
	{
		if (args.size() < 2)
		{
			result = "461 JOIN :Not enough parameters\r\n";
			break;
		}

		auto it = std::find_if(
			server->getChannels().begin(),
			server->getChannels().end(),
			[&](const std::shared_ptr<Channel> &ch)
			{
				return ch->getName() == args[1];
			});

		if (it == server->getChannels().end())
		{
			auto itClient = server->getClients().begin();
			std::shared_ptr<Channel> newChannel = std::make_shared<Channel>(
				args[1], itClient->second);
			server->addChannel(newChannel);
		}
		result = ":" + client.getNick() + " JOIN " + args[1] + "\r\n";
	}
	break;
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