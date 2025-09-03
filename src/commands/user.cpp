#include "commands.hpp"

std::string handleUser(Client &client, const std::vector<std::string> &args)
{
	std::string result = "";
	client.setUser(args[1], args[4].substr(1)); // get rid of ':'
	if (client.isRegistered())
	{
		std::string nick = client.getNick();
		result = ":ircserv 001 " + nick + " :Welcome to the IRC network\r\n";
		result += ":ircserv 002 " + nick + " :Your host is ircserv\r\n";
		result += ":ircserv 003 " + nick + " :This server was created today\r\n";
		result += ":ircserv 004 " + nick + " ircserv 1.0 iowghraAsORTVSxNCWqBzK\r\n";
	}

	std::cout << "Handling user: " << result << std::endl;
	return result;
}
