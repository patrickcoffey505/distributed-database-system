OBJ_DIR = obj
BIN_DIR = bin

.PHONY: clean all

all:
	$(MAKE) -C lib/kvstore
	$(MAKE) -C src/server
	$(MAKE) -C src/client
	$(MAKE) -C benchmark

clean:
	$(MAKE) -C lib/kvstore clean
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: kvstore client benchmark server

kvstore:
	$(MAKE) -C lib/kvstore

client:
	$(MAKE) -C src/client

server:
	$(MAKE) -C lib/kvstore
	$(MAKE) -C src/server

benchmark:
	$(MAKE) -C src/client
	$(MAKE) -C benchmark