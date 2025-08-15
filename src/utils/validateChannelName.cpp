#include "../irc.hpp"
#include <string>
#include <set>
#include <algorithm>
#include <cctype>

bool isValidChannelName(const std::string &name)
{
    if (name.empty() || name[0] != '#')
        return false;

    std::string channelName = name.substr(1);

    if (channelName.empty())
        return false;

    for (char c : channelName)
    {
        if (c == ' ' || c == '\a' || c == ',')
            return false;
    }

    auto toLower = [](const std::string &str) -> std::string
    {
        std::string res = str;
        std::transform(res.begin(), res.end(), res.begin(), ::tolower);
        return res;
    };

    std::string lowerName = toLower(name);

    // TODO: Check for existing channels
    // TODO: if there are no users in a channel, delete the channel and free the name

    return true;
}
