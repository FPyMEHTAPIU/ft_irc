#include "../irc.hpp"

int validateArgs(int argc, char **argv) {
	if (argc != 3) {
        throw std::invalid_argument("Usage: ./ircserv <port> <password>");
    }

	int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        throw std::out_of_range("Invalid port number. Must be between 1 and 65535.");
    }
	
	return port;
}