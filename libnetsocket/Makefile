# === Variables ===
CC = gcc
SRC_DIR = src
OBJ_DIR = build
INC = -I./include
CFLAGS = -Wall -Werror -Wextra
DEBUG = -fsanitize=address -g
LIB = libnetsocket.a

TARGET = netsocket
SERVER = server
CLIENT = client

SRC = $(addprefix $(SRC_DIR)/, main.c netsocket.c)
SERVER_SRC = $(addprefix $(SRC_DIR)/, server.c netsocket.c)
CLIENT_SRC = $(addprefix $(SRC_DIR)/, client.c netsocket.c)
LIB_SRC = $(SRC_DIR)/netsocket.c

OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
SERVER_OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SERVER_SRC))
CLIENT_OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CLIENT_SRC))
LIB_OBJ = $(OBJ_DIR)/netsocket.o


# === Default Target ===
all: $(TARGET)


# === Linking ===
$(TARGET): $(OBJ)
	$(CC) $(DEBUG) $^ -o $@

$(SERVER): $(SERVER_OBJ)
	$(CC) $(DEBUG) $^ -o $@

$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(DEBUG) $^ -o $@

# === Build static library ===
$(LIB): $(LIB_OBJ)
	ar rcs $@ $^

# === Compile each .c into build/%.o ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@



# === Clean ===
clean:
	rm -rf $(OBJ_DIR) *.a

fclean: clean
	rm -f $(TARGET) $(SERVER) $(CLIENT)

re: fclean all

.PHONY: all clean fclean re
