#include "commands.hpp"

std::string handleInvite(Server *server, const std::vector<std::string> &args, Client &client)
{
    // Check registration
    if (!client.isRegistered())
        return "451 " + client.getNick() + " :You have not registered\r\n";

    // if (args.size() < 3)
    //     return "461 " + client.getNick() + " INVITE :Not enough parameters\r\n";

    std::string targetNick = args[1];
    std::string channelName = args[2];

    // Validate channel name
    if (!isValidChannelName(channelName))
        return "403 " + client.getNick() + " " + channelName + " :No such channel\r\n";

    auto &channels = server->getChannels();
    auto channelIt = channels.find(channelName);

    // Channel must exist
    if (channelIt == channels.end())
        return "403 " + client.getNick() + " " + channelName + " :No such channel\r\n";

    std::shared_ptr<Channel> channel = channelIt->second;

    // Client must be on the channel to invite others
    if (!channel->hasUser(client))
        return "442 " + client.getNick() + " " + channelName + " :You're not on that channel\r\n";

    // Check if client has operator privileges if channel requires it
    if (channel->getIsInviteOnly() && !channel->isOperator(client))
        return "482 " + client.getNick() + " " + channelName + " :You're not channel operator\r\n";

    // Find the target client
    Client *targetClient = server->findClientByNick(targetNick);
    if (!targetClient)
        return "401 " + client.getNick() + " " + targetNick + " :No such nick\r\n";

    // Check if target is already on the channel
    if (channel->hasUser(*targetClient))
        return "443 " + client.getNick() + " " + targetNick + " " + channelName + " :is already on channel\r\n";

    return ("all good");
}
