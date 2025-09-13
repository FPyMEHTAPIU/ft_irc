#pragma once
#include "../irc.hpp"

std::string handleNick(std::shared_ptr<Client> client, const std::string &nick);
std::string handleUser(std::shared_ptr<Client> client, const std::vector<std::string> &args, std::string msg);
std::string handleJoin(Server *server, const std::vector<std::string> &args, std::shared_ptr<Client> client);