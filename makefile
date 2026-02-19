CC := gcc
CFLAGS := -Wall -Wextra -O2 -Iinclude -Ivendor/cglm/include -Ivendor/stb -Ivendor
LDFLAGS := -lglfw -lGLEW -lGL -lm
DEBUG_FLAGS := -g -O0 -DDEBUG

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Find all .c files in src directory
SOURCES := $(shell find $(SRC_DIR) -type f -name "*.c")
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLE := $(BIN_DIR)/tinycraft

# Default target
all: $(EXECUTABLE)

# Create binary
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)/.tinycraft
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@cp -r rsc/* $(BIN_DIR)/.tinycraft/
	@echo "Build complete: $@"

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Debug build with symbols
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(EXECUTABLE)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned build artifacts"

# Run the executable
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Debug run
debug-run: debug
	gdb ./$(EXECUTABLE)

# Phony targets
.PHONY: all debug clean run debug-run

# Dependency tracking (for future use with header dependencies)
-include $(OBJECTS:.o=.d)
