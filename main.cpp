#include <exception>
#include <csignal>
#include <string>
#include <iostream>

int main(int argc, char **argv) {
	std::signal(SIGINT, SIG_IGN);
	std::signal(SIGQUIT, SIG_IGN);
	std::string prompt;
	
	(void)argv;
	if (argc != 2)
		return 1;
	try {		
		while (true) {
			std::cout << "test" << std::flush;
			if (!std::getline(std::cin, prompt)) {
				break;
			}
		}
		std::cout << "bye" << std::endl;
		return 0;
	}
	catch (const std::exception& e) {
		std::cout << "An exception occurred: " << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cout << "An unknown error occurred." << std::endl;
		return 1;
	}
}