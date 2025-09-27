#include "commands.hpp"

std::string handleKick(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client, std::string reason)
{
    std::string nickname = client->getNick();

    if (!client->isRegistered())
        return "451 KICK :You have not registered\r\n";

    if (args.size() < 3)
        return "461 " + nickname + " KICK :Not enough parameters\r\n";

    std::string channelName = args.at(1);
    std::string targetNick = args.at(2);
    if (reason.empty())
        reason = "Kicked";

    std::shared_ptr<Channel> channel;
    try
    {
        channel = server->getChannelByName(channelName);
    }
    catch (...)
    {
        return "403 " + nickname + " " + channelName + " :No such channel\r\n";
    }

    if (!channel->hasUser(client))
        return "442 " + nickname + " " + channelName + " :You're not on that channel\r\n";

    if (!channel->isOperator(client))
        return "482 " + nickname + " " + channelName + " :You're not channel operator\r\n";

    std::shared_ptr<Client> target;
    try
    {
        target = server->getClientByNick(targetNick, nickname);
    }
    catch (...)
    {
        return "401 " + nickname + " " + targetNick + " :No such nick\r\n";
    }

    if (!channel->hasUser(target))
        return "441 " + nickname + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";

    std::string prefix = generatePrefix(client);
    std::string msg = prefix + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";

    channel->broadcast(server, msg, -1);
    channel->removeUser(target);

    if (channel->isEmpty())
        server->removeChannel(channelName);

    return "";
}
