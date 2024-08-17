CC = gcc
CFLAGS = -Wall -Wextra -g -I../src/client -I/opt/homebrew/Cellar/xxhash/0.8.2/include
LDFLAGS = -L/opt/homebrew/Cellar/xxhash/0.8.2/lib -lxxhash

SRC_DIR = .
OBJ_DIR = ../obj
BIN_DIR = ../bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o) ../obj/client_lib.o
EXEC = $(BIN_DIR)/benchmark

.PHONY: all

all: $(EXEC)

$(EXEC): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@