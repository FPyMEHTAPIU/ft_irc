#include "irc.hpp"

void handleInput(const std::string &input, Server *server)
{
	if (input.empty())
		return;

	std::vector<std::string> args = split(input, ' ');
	validateCommand(args[0]);

	switch (hash(input.c_str(), input.size()))
	{
	case hash("pass"):
		break;
	case hash("nick"):
		break;
	case hash("user"):
		break;
	case hash("join"):
	{
		std::cout << "privet" << std::endl;
		return;
		auto it = std::find_if(
				server->getChannels().begin(),
				server->getChannels().end(),
				[&](const std::shared_ptr<Channel> &ch)
				{
					return ch->getName() == args[1];
				});

		if (it == server->getChannels().end())
		{
			std::shared_ptr<Channel> newChannel = std::make_shared<Channel>(args[1], *server->getClients().begin());
			server->addChannel(newChannel);
		}
	}
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
	default:
		break;
	}
}