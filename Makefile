# Makefile for Chat Application

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lpthread

# Directories
CLIENT_DIR = client
SERVER_DIR = server
COMMON_DIR = common

# Source files
CLIENT_SRC = $(CLIENT_DIR)/main.c
SERVER_SRC = $(SERVER_DIR)/main.c
COMMON_HEADERS = $(COMMON_DIR)/packet.h

# Output executables
CLIENT_EXEC = client/client
SERVER_EXEC = server/server

# Default target
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Build client
$(CLIENT_EXEC): $(CLIENT_SRC) $(COMMON_HEADERS)
	@echo "Building client..."
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT_EXEC) $(LDFLAGS)

# Build server
$(SERVER_EXEC): $(SERVER_SRC) $(COMMON_HEADERS)
	@echo "Building server..."
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER_EXEC) $(LDFLAGS)

# Build only client
client: $(CLIENT_EXEC)

# Build only server
server: $(SERVER_EXEC)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC)
	rm -f $(CLIENT_DIR)/*.o $(SERVER_DIR)/*.o

# Install dependencies (if needed)
install-deps:
	@echo "Installing dependencies..."
	# Add any package installation commands here if needed
	# For example: sudo apt-get install build-essential

# Run server
run-server: $(SERVER_EXEC)
	@echo "Starting server..."
	./$(SERVER_EXEC)

# Run client
run-client: $(CLIENT_EXEC)
	@echo "Starting client..."
	./$(CLIENT_EXEC)

# Show help
help:
	@echo "Available targets:"
	@echo "  all          - Build both client and server (default)"
	@echo "  client       - Build only the client"
	@echo "  server       - Build only the server"
	@echo "  clean        - Remove build artifacts"
	@echo "  run-server   - Build and run the server"
	@echo "  run-client   - Build and run the client"
	@echo "  install-deps - Install build dependencies"
	@echo "  help         - Show this help message"

# Phony targets (don't represent actual files)
.PHONY: all client server clean install-deps run-server run-client help 