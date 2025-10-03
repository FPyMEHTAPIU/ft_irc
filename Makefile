# Colors
RESET 			= \033[0;39m
BLACK			= \033[1;30m
DARK_RED		= \033[1;31m
DARK_GREEN		= \033[1;32m
DARK_YELLOW		= \033[1;33m
DARK_BLUE		= \033[1;34m
DARK_MAGENTA	= \033[1;35m
DARK_CYAN		= \033[1;36m
DARK_GRAY		= \033[0;90m
LIGHT_GRAY		= \033[1;37m
RED				= \033[1;91m
ORANGE 			= \e[0m\e[38;5;202m
ORANGE_BOLD 	= \e[1m\e[38;5;202m
YELLOW 			= \033[0;93m
YELLOW_BOLD		= \033[1;93m
GREEN			= \033[1;92m
BLUE 			= \033[0;94m
BLUE_BOLD 		= \033[1;94m
CYAN 			= \033[0;96m
CYAN_BOLD 		= \033[1;96m
MAGENTA 		= \033[1;95m
PINK			= \e[1m\e[38;5;212m
WHITE 			= \033[1;97m

COMPILING		= @echo "📥 $(ORANGE)Compiling program files...$(RESET)"
LINKING			= @echo "📝 $(YELLOW)Linking targets...$(RESET)"
READY       	= @echo "🕹️ $(GREEN) Success! Run with ./$(NAME) [port] [password]$(RESET)"
CLEANING		= @echo "💧 $(CYAN)Cleaning...$(RESET)"
CLEANED			= @echo "💧 $(CYAN_BOLD)Successfully cleaned all object files!$(RESET)"
FCLEANING		= @echo "🧼 $(BLUE)Deep cleaning...$(RESET)"
FCLEANED		= @echo "🧼 $(BLUE_BOLD)Successfully cleaned all executable files!$(RESET)"
REMADE			= @echo "✅ $(GREEN)Successfully cleaned and rebuilt everything!$(RESET)"

NAME 			= ircserv

# Directories
SRC_DIR			=	src
OBJ_DIR			=	obj
INCLUDE_DIR		= include

CHANNEL_DIR		= channel
CLIENT_DIR		= client
COMMANDS_DIR	= commands
LOGGER_DIR		= logger
VALIDATION_DIR	= validation
SERVER_DIR		= server
UTILS_DIR		= utils
COMMON_DIR		= common

CHANNEL_FILES = Channel.cpp
CLIENT_FILES = Client.cpp
LOGGER_FILES = Logger.cpp
SERVER_FILES = Server.cpp

COMMANDS_FILES = \
	invite.cpp \
	join.cpp \
	kick.cpp \
	mode.cpp \
	nick.cpp \
	part.cpp \
	pass.cpp \
	ping.cpp \
	privmsg.cpp \
	topic.cpp \
	user.cpp \
	who.cpp \
	whois.cpp

VALIDATION_FILES = \
	validation.cpp

UTILS_FILES = \
	responsePrefix.cpp \
	utils.cpp

COMMON_FILES = \
	socket_utils.cpp

SRC_FILES = \
	$(addprefix $(CHANNEL_DIR)/, $(CHANNEL_FILES)) \
	$(addprefix $(CLIENT_DIR)/, $(CLIENT_FILES)) \
	$(addprefix $(COMMANDS_DIR)/, $(COMMANDS_FILES)) \
	$(addprefix $(LOGGER_DIR)/, $(LOGGER_FILES)) \
	$(addprefix $(VALIDATION_DIR)/, $(VALIDATION_FILES)) \
	$(addprefix $(SERVER_DIR)/, $(SERVER_FILES)) \
	$(addprefix $(UTILS_DIR)/, $(UTILS_FILES)) \
	$(addprefix $(COMMON_DIR)/, $(COMMON_FILES)) \
	handleInput.cpp \
	main.cpp \

SRC = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Object files
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

CXX = c++
CPPFLAGS += -I$(INCLUDE_DIR)
CXXFLAGS += -std=c++20 -Wall -Werror -Wextra -g

RM = rm -rf

.PHONY: all clean fclean re test

all: $(NAME)

$(NAME): $(OBJ)
	$(COMPILING)
	@$(CXX) $(CXXFLAGS) $^ -o $@
	$(LINKING)
	@chmod +x $@
	$(READY)

# Compile .cpp -> .o and create obj directories as needed
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(CLEANING)
	@$(RM) $(OBJ_DIR)
	$(CLEANED)

fclean: clean
	$(FCLEANING)
	@$(RM) $(NAME)
	$(FCLEANED)

re: fclean all

test: all
	@cmake -S . -B build
	@cmake --build build
	@(cd build && ctest --output-on-failure)
