#pragma once
#include "../irc.hpp"

std::string handleNick(Client &client, const std::string &nick);
std::string handleUser(Client &client, const std::vector<std::string> &args, std::string msg);
std::string handleJoin(Server *server, const std::vector<std::string> &args, Client &client);