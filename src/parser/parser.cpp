#include "../irc.hpp"

void validatePassword(const std::string& password) {
    if (password.empty() || password.length() < PWD_MIN_LENGTH) {
        throw std::out_of_range("Password must have at least " + std::to_string(PWD_MIN_LENGTH));
    } 
    else if (password.length() > PWD_MAX_LENGTH) {
        throw std::out_of_range("Password can have maximum " + std::to_string(PWD_MAX_LENGTH));
    } 
    else if (password.find_first_of(" \t\n\r") != std::string::npos) {
        throw std::invalid_argument("Password cannot contain whitespaces");
    }
}

int validatePort(std::string strPort) {
    int port = std::stoi(strPort);
    if (port <= 0 || port > 65535) {
        throw std::out_of_range("Invalid port number. Must be between 1 and 65535.");
    }

    return port;
}

int validateArgs(int argc, char **argv) {
	if (argc != 3) {
        throw std::invalid_argument("Usage: ./ircserv <port> <password>");
    }

	int port = validatePort(argv[1]);
    validatePassword(argv[2]);
	
	return port;
}