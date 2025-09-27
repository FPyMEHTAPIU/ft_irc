#include "../irc.hpp"

void validatePassword(const std::string &password)
{
    if (password.empty())
    {
        throw std::out_of_range("Password cannot be empty");
    }
    else if (password.length() < PASSWORD_MIN_LENGTH)
    {
        throw std::out_of_range("Password must have at least " + std::to_string(PASSWORD_MIN_LENGTH) + " characters");
    }
    else if (password.length() > PASSWORD_MAX_LENGTH)
    {
        throw std::out_of_range("Password can have a maximum of " + std::to_string(PASSWORD_MAX_LENGTH) + " characters");
    }
    else if (password.find_first_of(" \t\n\r") != std::string::npos)
    {
        throw std::invalid_argument("Password cannot contain whitespaces");
    }
}

void validateClientPassword(const std::string &inputPassword, const std::string &storedPassword)
{
    std::string parsedInputPassword = inputPassword;
    parsedInputPassword.erase(0, parsedInputPassword.find_first_not_of(" \t\r\n"));
    parsedInputPassword.erase(parsedInputPassword.find_last_not_of(" \t\r\n") + 1);

    std::string parsedStoredPassword = storedPassword;
    parsedStoredPassword.erase(0, parsedStoredPassword.find_first_not_of(" \t\r\n"));
    parsedStoredPassword.erase(parsedStoredPassword.find_last_not_of(" \t\r\n") + 1);

    validatePassword(parsedInputPassword);

    if (parsedInputPassword != parsedStoredPassword)
    {
        throw std::invalid_argument("Password incorrect");
    }
}

// Allowed special characters in nicknames: -[]\\`^{}
// First character must be a letter or one of the specials
// Allowed after first character: letters, digits and specials
// Not allowed: spaces, control characters, @, !
bool isValidNickname(const std::string &nickName)
{
    const size_t MIN_LEN = 1;
    const size_t MAX_LEN = 16; // RFC says 9, modern servers may allow more

    if (nickName.size() < MIN_LEN || nickName.size() > MAX_LEN)
        return false;

    const std::string specials = "-[]\\`^{}";

    unsigned char first = static_cast<unsigned char>(nickName[0]);
    if (!(std::isalpha(first) || specials.find(nickName[0]) != std::string::npos))
        return false;

    for (size_t i = 0; i < nickName.size(); ++i)
    {
        unsigned char ch = static_cast<unsigned char>(nickName[i]);
        if (std::isalpha(ch) || std::isdigit(ch) || specials.find(ch) != std::string::npos)
        {
            continue;
        }
        if (std::isspace(ch) || ch == '@' || ch == '!' || ch == ',' || ch < 0x20)
        {
            return false;
        }
        // Any other character not in allowed set is invalid
        return false;
    }
    return true;
}

// Allowed characters in usernames: letters, digits, - (dash), _ (underscore), . (dot)
// Not allowed: spaces, control characters, @, !
bool isValidUsername(const std::string &userName)
{
    const size_t MIN_LEN = 1;
    const size_t MAX_LEN = 12; // RFC says 10, many servers allow up to 30

    if (userName.size() < MIN_LEN || userName.size() > MAX_LEN)
        return false;

    for (size_t i = 0; i < userName.size(); ++i)
    {
        unsigned char ch = static_cast<unsigned char>(userName[i]);

        if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.')
        {
            continue;
        }
        if (std::isspace(ch) || ch == '@' || ch == '!' || ch == ',' || ch < 0x20)
        {
            return false;
        }
        // Any other character not in allowed set is invalid
        return false;
    }
    return true;
}

int validatePort(const std::string &strPort)
{
    if (strPort.empty())
    {
        throw std::invalid_argument("Port cannot be empty");
    }
    for (unsigned char c : strPort)
    {
        if (!std::isdigit(c))
        {
            throw std::invalid_argument("Port must contain only numbers");
        }
    }
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
        "pass", "nick", "user", "join", "privmsg", "mode",
        "invite", "kick", "topic", "quit", "msg", "cap",
        "ping", "who", "part"};

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