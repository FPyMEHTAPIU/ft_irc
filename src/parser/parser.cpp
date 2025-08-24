#include "../irc.hpp"

void validatePassword(const std::string &password)
{
    if (password.empty() || password.length() < PWD_MIN_LENGTH)
    {
        throw std::out_of_range("Password must have at least " + std::to_string(PWD_MIN_LENGTH));
    }
    else if (password.length() > PWD_MAX_LENGTH)
    {
        throw std::out_of_range("Password can have maximum " + std::to_string(PWD_MAX_LENGTH));
    }
    else if (password.find_first_of(" \t\n\r") != std::string::npos)
    {
        throw std::invalid_argument("Password cannot contain whitespaces");
    }
}

int validatePort(const std::string &strPort)
{
    int port = std::stoi(strPort);
    if (port <= 0 || port > 65535)
    {
        throw std::out_of_range("Invalid port number. Must be between 1 and 65535.");
    }

    return port;
}

int validateArgs(int argc, char **argv)
{
    if (argc != 3)
    {
        throw std::invalid_argument("Usage: ./ircserv <port> <password>");
    }

    int port = validatePort(argv[1]);
    validatePassword(argv[2]);

    return port;
}

void validateCommand(const std::string &command)
{
    const std::set<std::string> cmds = {
        "pass", "nick", "user", "join", "part", "privmsg", "notice",
        "mode", "invite", "kick", "topic", "names", "list", "oper",
        "kill", "quit", "msg", "cap"};

    std::string cmdLowercase(command.size(), '\0');
    std::transform(command.begin(), command.end(),
                   cmdLowercase.begin(), ::tolower);

    for (const std::string &cmd : cmds)
    {
        if (cmd == cmdLowercase)
            return;
    }
    throw std::invalid_argument("Invalid command: " + command);
}