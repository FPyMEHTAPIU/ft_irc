#include "commands.hpp"

std::string handleUser(Client &client, const std::vector<std::string> &args)
{
	std::string result = "";
	client.setUser(args[1], args[4].substr(1)); // get rid of ':'
	if (client.isRegistered())
	{
		result = ":ircserv 001 " + client.getNick() + " :Welcome to the IRC network\r\n";
		result += ":ircserv 002 " + client.getNick() + " :Your host is ircserv\r\n";
		result += ":ircserv 003 " + client.getNick() + " :This server was created today\r\n";
	}
	return result;
}
