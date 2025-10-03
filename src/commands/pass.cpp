#include "commands.hpp"
#include "../logger/Logger.hpp"
#include "../validation/validation.hpp"

std::string handlePass(Server *server, std::shared_ptr<Client> client, int clientFd,
                       const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        return "461 PASS :Not enough parameters\r\n";
    }

    const std::string &providedPassword = args[1];
    std::string storedPassword = server->getPassword();

    try
    {
        server->logger->debug(AUTH, "Validating password...");
        Validation::validateClientPassword(providedPassword, storedPassword);
        client->authenticate();
        server->logger->info(AUTH, "Authenticated " + client->getNick() + "!");
        return "";
    }
    catch (const std::exception &e)
    {
        client->enqueueMessage("464 PASS :Password incorrect\r\n");
        server->enableWrite(clientFd);

        server->logger->error(AUTH, "Password validation failed for client FD " + std::to_string(clientFd));

        return "";
    }
}