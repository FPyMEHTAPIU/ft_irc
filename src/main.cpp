#include <exception>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Server.hpp"
#include "irc.hpp"

int main(int argc, char* argv[]) {

	if (!validateArgs(argc, argv)) {
		return 1;
	}

    int port = std::atoi(argv[1]);
	std::string password = argv[2];

	if (!parser(port, password)) {
		return 1;
	}

    try {
        Server server(port, password);
        server.start();
        // server.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}