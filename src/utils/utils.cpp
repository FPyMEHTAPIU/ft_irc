#include "../irc.hpp"

bool startsWith(std::string str, std::string symbols)
{
    char firstChar = str[0];
    for (char c : symbols)
    {
        if (firstChar == c)
        {
            return true;
        }
    }
    return false;
}

std::string strToLowercase(const std::string &str)
{
    std::string strLowercase(str.size(), '\0');
    std::transform(str.begin(), str.end(),
                   strLowercase.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    return strLowercase;
}

std::vector<std::string> split(const std::string &sentence, char sep)
{
    std::vector<std::string> vec;
    size_t start;
    size_t end = 0;

    while ((start = sentence.find_first_not_of(sep, end)) != std::string::npos)
    {
        end = sentence.find(sep, start);
        vec.push_back(sentence.substr(start, end - start));
    }
    return vec;
}