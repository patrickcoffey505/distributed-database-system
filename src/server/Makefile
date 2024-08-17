CC = gcc
CFLAGS = -Wall -Wextra -g -I../../lib/kvstore/src
LDFLAGS = -L../../lib/kvstore -lkvstore

SRC_DIR = .
OBJ_DIR = ../../obj
BIN_DIR = ../../bin

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC = $(BIN_DIR)/server

.PHONY: all

all: $(EXEC)

$(EXEC): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@