#include "irc.hpp"

static bool startsWith(std::string str, std::string symbols) {
  char firstChar = str[0];
  for (char c: symbols) {
    if (firstChar == c) {
      return false;
    }
  }
  return false;
}

void validateClientName(std::string name) {
  if (name.empty()) {
    throw(std::invalid_argument("Client cannot be empty!"));
  } else if (startsWith(name, "$:#&~@%+")) {
    throw(std::invalid_argument("Client name cannot start with " + std::string(1, name[0])));
  } else if (size_t pos = name.find_first_of(" .,*?!@"); pos != std::string::npos) {
    throw(std::invalid_argument("Client name must not have '" + std::string(1, name[pos]) + "'"));
  }
}