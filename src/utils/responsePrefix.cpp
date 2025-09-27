#include "../irc.hpp"

std::string generatePrefix(std::shared_ptr<Client> client)
{
    return ":" + client->getNick() + "!" + client->getUsername() + "@localhost";
}
