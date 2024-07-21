CC = gcc
CFLAGS = -Wall -Wextra -g -I./lib/kvstore/src
LDFLAGS = -L./lib/kvstore -lkvstore

# Directories
SRC_DIR = benchmark
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib/kvstore

# Files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC = $(BIN_DIR)/benchmark

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ) | $(BIN_DIR)
	$(MAKE) -C $(LIB_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(MAKE) -C $(LIB_DIR) clean
	rm -rf $(OBJ_DIR) $(BIN_DIR)