#include "../irc.hpp"

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
