#include "commands.hpp"

std::string handleTopic(Server *server, const std::vector<std::string> &args,
                        std::string newTopic, std::shared_ptr<Client> client)
{
    if (args.size() < 1)
        return "461 TOPIC :Not enough parameters\r\n";

    std::string channelName = args.at(1);
    std::shared_ptr<Channel> channel;

    try
    {
        channel = server->getChannelByName(channelName);
    }
    catch (...)
    {
        return "403 " + client->getNick() + " " + channelName + " :No such channel\r\n";
    }

    // If only 2 args → query topic
    if (args.size() == 1)
    {
        if (channel->getTopic().empty())
            return "331 " + client->getNick() + " " + channelName + " :No topic is set\r\n";
        else
            return "332 " + client->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
    }

    // Remove leading colon if present
    if (!newTopic.empty() && newTopic[0] == ':')
        newTopic = newTopic.substr(1);

    if (channel->isTopicRestricted() && !channel->isOperator(client))
        return "482 " + client->getNick() + " " + channelName + " :You're not channel operator\r\n";

    channel->setTopic(newTopic);

    std::string msg = ":" + client->getNick() + " TOPIC " + channelName + " :" + newTopic + "\r\n";

    messageInfo msgInfo = {
        channel->getName(),
        client,
        client->getNick(),
        client->getFd(),
        msg};

    channel->broadcast(server, msgInfo);
    return msg;
}
