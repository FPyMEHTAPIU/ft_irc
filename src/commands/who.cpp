#include "commands.hpp"

static std::string handleWhoChannel(Server *server, std::string mask, std::string senderNick)
{
    std::string response = "";

    try
    {
        std::shared_ptr<Channel> channel = server->getChannelByName(mask);
        for (auto &member : channel->getUsers())
        {
            std::string flags = "H";
            if (channel->isOperator(member))
            {
                flags += "@";
            }

            response += ":ircserv 352 " + senderNick + " " + mask + " " +
                        member->getUsername() + " localhost ircserv " +
                        member->getNick() + " " +
                        flags + " :0 " +
                        member->getRealName() + "\r\n";
        }
        return response;
    }
    catch (std::exception &e)
    {
        return e.what();
    }
}

static std::string handleWhoClient(Server *server, std::string mask, std::string senderNick)
{
    std::string response = "";

    try
    {
        std::shared_ptr<Client> target = server->getClientByNick(mask, senderNick);
        std::string flags = "H";
        response = ":ircserv 352 " + senderNick + " " +
                   target->getUsername() + " localhost ircserv " +
                   target->getNick() + " " +
                   flags + " :0 " +
                   target->getRealName() + "\r\n";
        return response;
    }
    catch (std::exception &e)
    {
        return e.what();
    }
}

std::string handleWho(Server *server, const std::vector<std::string> &args,
                      std::shared_ptr<Client> client)
{
    std::string senderNick = client->getNick();

    if (args.size() < 2)
        return "461 WHO :Not enough parameters\r\n";

    std::string mask = args.at(1);
    std::string response = "";

    if (mask.at(0) == '#')
    {
        response = handleWhoChannel(server, mask, senderNick);
        if (response.starts_with("403"))
            return response;
    }
    else
    {
        response = handleWhoClient(server, mask, senderNick);
        if (response.starts_with("401"))
            return response;
    }

    response += ":ircserv 315 " + senderNick + " " + mask + " :End of WHO list\r\n";
    return response;
}