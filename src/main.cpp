#include <exception>
#include <string>
#include <iostream>
#include <cstdlib>

#include "Server.hpp"
#include "irc.hpp"

int main(int argc, char *argv[])
{
    try
    {
        int port = validateArgs(argc, argv);

        std::string password = argv[2];
        validatePassword(password);
        Logger logger;
        Server server(port, password, &logger);
        server.start();
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << ANSI_RED << "Error\n"
                  << ANSI_RESET << e.what() << std::endl;
        return 1;
    }

    return 0;
}