#include "commands.hpp"
#include <ctime>

std::string handleWhois(Server *server, const std::vector<std::string> &args,
                        std::shared_ptr<Client> client)
{
    std::string senderNick = client->getNick();
    if (args.size() < 2)
        return "461 WHOIS :Not enough parameters\r\n";

    std::string targetNick = args.at(1);
    std::string response = "";

    try
    {
        std::shared_ptr<Client> target = server->getClientByNick(targetNick, senderNick);

        response += ":ircserv 311 " + senderNick + " " +
                    target->getNick() + " " +
                    target->getUsername() + " " +
                    "localhost ircserv " +
                    ":" + target->getRealName() + "\r\n";

        response += ":ircserv 312 " + senderNick + " " +
                    target->getNick() + " ircserv :IRC Server\r\n";

        response += ":ircserv 317 " + senderNick + " " +
                    target->getNick() + " 0 " +
                    std::to_string(std::time(nullptr)) + " :seconds idle, signon time\r\n";

        std::string chans = "";
        for (auto &ch : server->getChannels())
        {
            if (ch.second->hasUser(target))
            {
                if (!chans.empty())
                    chans += " ";
                chans += ch.second->getName();
            }
        }
        if (!chans.empty())
        {
            response += ":ircserv 319 " + senderNick + " " +
                        target->getNick() + " :" + chans + "\r\n";
        }

        response += ":ircserv 318 " + senderNick + " " +
                    target->getNick() + " :End of WHOIS list\r\n";

        return response;
    }
    catch (std::exception &e)
    {
        response += ":ircserv 401 " + senderNick + " " +
                    targetNick + " :No such nick\r\n";
        response += ":ircserv 318 " + senderNick + " " +
                    targetNick + " :End of WHOIS list\r\n";
        return response;
    }
}
