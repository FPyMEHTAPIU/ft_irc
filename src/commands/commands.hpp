#pragma once
#include "../irc.hpp"
#include "../validation/validation.hpp"

std::string handleNick(Server *server, std::shared_ptr<Client> client,
                       std::string &newNick);
std::string handleUser(std::shared_ptr<Client> client, const std::vector<std::string> &args,
                       std::string msg);
std::string handleJoin(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client);
void handlePrivmsg(Server *server, std::vector<std::string> args,
                   int senderFd, const std::string &message);
std::string handlePing(const std::vector<std::string> &args);
std::string handleTopic(Server *server, const std::vector<std::string> &args,
                        std::string newTopic, std::shared_ptr<Client> client);
std::string handleMode(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client);
std::string handleWho(Server *server, const std::vector<std::string> &args,
                      std::shared_ptr<Client> client);
std::string handleKick(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client, std::string reason);
std::string handlePart(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client, std::string reason);
std::string handleInvite(Server *server, const std::vector<std::string> &args,
                         std::shared_ptr<Client> client);
std::string handlePart(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client, std::string reason);
std::string handlePass(Server *server, std::shared_ptr<Client> client, int clientFd,
                       const std::vector<std::string> &args);
std::string handleWhois(Server *server, const std::vector<std::string> &args,
                        std::shared_ptr<Client> client);