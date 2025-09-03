#include "../irc.hpp"

std::string strToLowercase(const std::string &str)
{
	std::string strLowercase(str.size(), '\0');
	std::transform(str.begin(), str.end(),
				   strLowercase.begin(),
				   [](unsigned char c)
				   { return std::tolower(c); });

	return strLowercase;
}
