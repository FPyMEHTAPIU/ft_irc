#include "commands.hpp"

std::string handlePart(Server *server, const std::vector<std::string> &args,
                       std::shared_ptr<Client> client, std::string reason)
{
    std::string nickname = client->getNick();

    if (!client->isRegistered())
        return "451 PART :You have not registered\r\n";

    if (args.size() < 2)
        return "461 " + nickname + " PART :Not enough parameters\r\n";

    std::string channelName = args.at(1);

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

    std::string prefix = generatePrefix(client);
    std::string msg = prefix + " PART " + channelName;
    if (!reason.empty())
        msg += " :" + reason;
    msg += "\r\n";

    channel->broadcast(server, msg, -1);
    channel->removeUser(client);

    if (channel->isEmpty())
        server->removeChannel(channelName);

    return "";
}
