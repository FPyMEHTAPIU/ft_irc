#pragma once
#include "../irc.hpp"

std::string handleNick(std::shared_ptr<Client> client, const std::string &nick);
std::string handleUser(std::shared_ptr<Client> client, const std::vector<std::string> &args, std::string msg);
std::string handleJoin(Server *server, const std::vector<std::string> &args, std::shared_ptr<Client> client);
void handlePrivmsg(Server *server, std::vector<std::string> args, int senderFd, const std::string &message);
std::string handlePing(const std::vector<std::string> &args);
std::string handleTopic(Server *server, const std::vector<std::string> &args,
                        std::string newTopic, std::shared_ptr<Client> client);
std::string handleMode(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client);
std::string handleWho(Server *server, const std::vector<std::string> &args,
                      std::shared_ptr<Client> client);
std::string handleKick(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client, std::string reason);