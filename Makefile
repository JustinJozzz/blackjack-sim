# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = 

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(BUILD_DIR)/main.o
LIB_OBJECTS = $(filter-out $(MAIN_OBJ), $(OBJECTS))

# Test files
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_EXECUTABLES = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%)

# Targets
TARGET = blackjack
TEST_TARGET = test_game

# Default target
all: $(TARGET)

# Build main executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build object files from source
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test object files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Build test executables
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(LIB_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run tests
test: $(BUILD_DIR)/$(TEST_TARGET)
	./$(BUILD_DIR)/$(TEST_TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Optimized build for release
release: CFLAGS += -O3 -march=native -flto -DNDEBUG
release: clean all

# Debug build
debug: CFLAGS += -g -O0 -DDEBUG
debug: clean all

# Run the simulator
run: $(TARGET)
	./$(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make test     - Build and run tests"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make release  - Build optimized release version"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make run      - Build and run the simulator"

.PHONY: all clean test release debug run help