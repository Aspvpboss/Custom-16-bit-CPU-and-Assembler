CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
INCLUDE = -Isrc
OUT = assembler.exe

SRC = $(wildcard src/*.c)

# Defaults
BUILD_DIR = build/debug
BUILD_FLAGS = -g -DTRACK_ALLOCATIONS

OBJ = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRC))

.PHONY: debug_build opt_build clean



default: debug_run

opt: opt_run
	


debug_build: BUILD_DIR = build/debug
debug_build: BUILD_FLAGS = -g -DTRACK_ALLOCATIONS
debug_build: OBJ = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC))
debug_build: $(BUILD_DIR) $(OBJ)
	$(CC) $(CFLAGS) $(BUILD_FLAGS) $(INCLUDE) -o $(BUILD_DIR)/$(OUT) $(OBJ)

debug_run: debug_build
	@echo  
	@echo "Running debug build..."
	@echo
	$(BUILD_DIR)/$(OUT)


opt_build: BUILD_DIR = build/release
opt_build: BUILD_FLAGS = -O2 -DSTDLIB_ALLOCATIONS
opt_build: OBJ = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC))
opt_build: $(BUILD_DIR) $(OBJ)
	$(CC) $(CFLAGS) $(BUILD_FLAGS) $(INCLUDE) -o $(BUILD_DIR)/$(OUT) $(OBJ)

opt_run: opt_build
	@echo
	@echo "Running release build..."
	@echo
	$(BUILD_DIR)/$(OUT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(BUILD_FLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf build