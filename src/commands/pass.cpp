#include "commands.hpp"

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
        validateClientPassword(providedPassword, storedPassword);
        client->authenticate();
        server->logger->info(AUTH, "Authenticated " + client->getNick() + "!");
        return ""; // success: no explicit reply needed, let normal flow continue
    }
    catch (const std::exception &e)
    {
        // Build the error message BEFORE removing client
        std::string err = "464 PASS :Password incorrect\r\n";
        send(clientFd, err.c_str(), err.size(), 0); // flush error immediately

        server->logger->error(AUTH, "Password validation failed for client FD " + std::to_string(clientFd));

        // Now disconnect the client
        server->removeClient(clientFd);

        return ""; // no further reply (already sent + closed)
    }
}