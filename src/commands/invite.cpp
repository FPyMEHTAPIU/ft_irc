#include "commands.hpp"

std::string handleInvite(Server *server, const std::vector<std::string> &args, std::shared_ptr<Client> client)
{
    // Check registration
    if (!client->isRegistered())
        return "451 INVITE :You have not registered\r\n";

    // Check parameters
    if (args.size() < 3)
        return "461 INVITE :Not enough parameters\r\n";

    std::string targetNick = args.at(1);
    std::string channelName = args.at(2);
    std::string senderNick = client->getNick();

    // Validate channel name
    if (!Validation::isValidChannelName(channelName))
        return "476 " + senderNick + " " + channelName + " :Bad Channel Mask\r\n";

    // Check if channel exists
    std::shared_ptr<Channel> channel;
    try
    {
        channel = server->getChannelByName(channelName);
    }
    catch (...)
    {
        return "403 " + senderNick + " " + channelName + " :No such channel\r\n";
    }

    // Check if sender is on the channel
    if (!channel->hasUser(client))
        return "442 " + senderNick + " " + channelName + " :You're not on that channel\r\n";

    // Check if sender is operator (required for invite-only channels)
    if (channel->getIsInviteOnly() && !channel->isOperator(client))
        return "482 " + senderNick + " " + channelName + " :You're not channel operator\r\n";

    // Find target client
    std::shared_ptr<Client> targetClient = nullptr;
    for (const auto &clientPair : server->getClients())
    {
        if (clientPair.second->getNick() == targetNick)
        {
            targetClient = clientPair.second;
            break;
        }
    }

    if (!targetClient)
        return "401 " + senderNick + " " + targetNick + " :No such nick/channel\r\n";

    // Check if target is already on channel
    if (channel->hasUser(targetClient))
        return "443 " + senderNick + " " + targetNick + " " + channelName + " :is already on channel\r\n";

    // Add target to invite list
    channel->addInvite(targetClient);

    // Send invitation to target client
    std::string inviteMsg = ":" + senderNick + " INVITE " + targetNick + " " + channelName + "\r\n";
    targetClient->enqueueMessage(inviteMsg);
    server->enableWrite(targetClient->getFd());

    // Send confirmation to sender
    return "341 " + senderNick + " " + targetNick + " " + channelName + "\r\n";
}