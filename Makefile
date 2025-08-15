#colors
DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
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

UTILS_DIR = utils
PARSER_DIR = parser

UTILS_FILES = \
	validateChannelName.cpp \
	validateClientName.cpp

PARSER_FILES = \
	parser.cpp

SRC_FILES = \
	$(addprefix $(UTILS_DIR)/, $(UTILS_FILES)) \
	$(addprefix $(PARSER_DIR)/, $(PARSER_FILES)) \
	main.cpp \
	Channel.cpp \
	Client.cpp \
	Server.cpp

SRC				=	$(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Object files
OBJ 			=	$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

CXX = c++
CXXFLAGS = -std=c++20 -Wall -Werror -Wextra -g

RM = rm -rf

.PHONY = all clean fclean re test

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(BLUE)🛠  Compiling ircserv... 🛠$(DEF_COLOR)"
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)🥳 Success!🥳$(DEF_COLOR)"

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)🚽 Deleting object files... 🚽$(DEF_COLOR)"
	@$(RM) $(OBJ_DIR)

fclean: clean
	@echo "$(RED)🪦 Deleting ircserv... 🪦$(DEF_COLOR)"
	@$(RM) $(NAME)
	@echo "$(RED)☣️  CLEAR ☣️$(DEF_COLOR)"

re: fclean all

test: all
	@cmake -S . -B build
	@cmake --build build
	@(cd build && ctest --output-on-failure)
