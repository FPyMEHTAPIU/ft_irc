#include "commands.hpp"

std::string handlePing(const std::vector<std::string> &args)
{
    if (args.size() < 2)
        return "409 PING :No origin specified\r\n";

    std::string token = args.at(1);

    // Respond with PONG
    return "PONG :" + token + "\r\n";
}
