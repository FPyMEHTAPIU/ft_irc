#colors
DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
ORANGE = \e[0m\e[38;5;202m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m

NAME = ircserv

# Directories
SRC_DIR 		=	src
OBJ_DIR 		=	obj

CHANNEL_DIR = channel
CLIENT_DIR = client
COMMANDS_DIR = commands
PARSER_DIR = parser
UTILS_DIR = utils

CHANNEL_FILES = \
	Channel.cpp

CLIENT_FILES = \
	Client.cpp

COMMANDS_FILES = \
	join.cpp \
	nick.cpp \
	user.cpp

PARSER_FILES = \
	parser.cpp

UTILS_FILES = \
	split.cpp \
	strToLowercase.cpp \
	validateChannelName.cpp \
	validateClientName.cpp

SRC_FILES = \
	$(addprefix $(CHANNEL_DIR)/, $(CHANNEL_FILES)) \
	$(addprefix $(CLIENT_DIR)/, $(CLIENT_FILES)) \
	$(addprefix $(COMMANDS_DIR)/, $(COMMANDS_FILES)) \
	$(addprefix $(PARSER_DIR)/, $(PARSER_FILES)) \
	$(addprefix $(UTILS_DIR)/, $(UTILS_FILES)) \
	Server.cpp \
	inputHandle.cpp \
	main.cpp \
	Logger.cpp


SRC				=	$(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Object files
OBJ 			=	$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

CXX = c++
CXXFLAGS = -std=c++20 -Wall -Werror -Wextra -g

RM = rm -rf

.PHONY = all clean fclean re test

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(BLUE)🔨 Compiling ircserv...$(DEF_COLOR)"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)🥳 Success! Run with ./$(NAME) [port] [password]$(DEF_COLOR)"

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)🚽 Deleting object files...$(DEF_COLOR)"
	@$(RM) $(OBJ_DIR)

fclean: clean
	@echo "$(ORANGE)🪦 Deleting $(NAME)...$(DEF_COLOR)"
	@$(RM) $(NAME)
	@echo "$(RED)☣️  CLEAR!$(DEF_COLOR)"

re: fclean all

test: all
	@cmake -S . -B build
	@cmake --build build
	@(cd build && ctest --output-on-failure)
