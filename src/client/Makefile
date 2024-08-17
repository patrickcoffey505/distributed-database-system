CC = gcc
CFLAGS = -Wall -Wextra -g

SRC_DIR = .
OBJ_DIR = ../../obj
BIN_DIR = ../../bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC = $(BIN_DIR)/client

.PHONY: all

all: $(EXEC)

$(EXEC): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@