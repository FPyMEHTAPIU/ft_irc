#include "../irc.hpp"

bool validateArgs(int argc, char **argv) {
	if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return false;
    }
	return true;
}

bool parser(int port, std::string password) {
	if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number. Must be between 1 and 65535." << std::endl;
        return false;
    }
    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty." << std::endl;
        return false;
    }
	return true;
}