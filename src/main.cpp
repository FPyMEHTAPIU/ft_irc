#include <iostream>
#include "Channel.hpp"
#include "Client.hpp"

int main(void) {
	std::cout << "Hello world!\n";

	Client user;
	Channel channel("Best name ever", user);
	std::cout << "ID: " << channel.getId() << " name: " << channel.getName() << std::endl;
}