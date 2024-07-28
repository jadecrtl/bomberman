# Directories
OBJ_DIR = obj

SRCS = src/message.c \
	src/utils.c \
	src/message_send.c \
	src/message_convert.c \
	src/init.c \
	src/free.c \
	src/movement.c \
	src/bombe.c \

# Source files
SRCS_CLIENT = src/client.c $(SRCS) src/gui.c
SRCS_SERVER = src/serveur.c $(SRCS)

# Object files
OBJS_CLIENT = $(SRCS_CLIENT:%.c=$(OBJ_DIR)/%.o)
OBJS_SERVER = $(SRCS_SERVER:%.c=$(OBJ_DIR)/%.o)

# Program names
CLIENT_NAME = client
SERVER_NAME = server

# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -I./includes -g3

RM = rm -rf

# Rule to compile .c files to .o files and move them to the obj directory
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Build the programs
all: $(CLIENT_NAME) $(SERVER_NAME)

$(CLIENT_NAME): $(OBJS_CLIENT)
	$(CC) -o $(CLIENT_NAME) $(OBJS_CLIENT) -lncurses

$(SERVER_NAME): $(OBJS_SERVER)
	$(CC) -o $(SERVER_NAME) $(OBJS_SERVER)

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(CLIENT_NAME) $(SERVER_NAME)

re: fclean all

.PHONY: all clean fclean re
