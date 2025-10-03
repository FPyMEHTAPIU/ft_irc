#pragma once

// ANSI Color Codes
#define ANSI_RED "\033[0;91m"
#define ANSI_ORANGE "\e[0m\e[38;5;202m"
#define ANSI_YELLOW "\033[0;93m"
#define ANSI_GREEN "\033[0;92m"
#define ANSI_BLUE "\e[0m\e[38;5;39m"
#define ANSI_PURPLE "\e[0m\e[38;5;171m"
#define ANSI_RESET "\033[0;39m"

// IRC Protocol Constants
#define PASSWORD_MIN_LENGTH 5
#define PASSWORD_MAX_LENGTH 32

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

#define SERVER_BACKLOG 10 // max number of pending connections
#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK 2
#endif
