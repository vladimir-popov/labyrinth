.DEFAULT_GOAL = compile

CC = gcc
CFLAGS = -Wall -g # Turn on debug info

BUILD_DIR := ./build
SRC_DIR := ./src
TEST_DIR := ./test

APP_MAIN := app.c
TEST_MAIN := all_tests.c

APP_EXEC := labyrinth
TEST_EXEC := run_tests

# Find all the C files we want to compile, except the source with main function
SRCS := $(shell find $(SRC_DIR) -name '*.c' -and -not -name $(APP_MAIN))

# Find all C files with tests
TEST_SRCS := $(shell find $(TEST_DIR) -name '*.c')

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./src/hello.c turns into ./build/./src/hello.c.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# Concatenate src with test, but without APP_MAIN
TEST_OBJS := $(OBJS)

# Add APP_MAIN to build
OBJS += $(BUILD_DIR)/$(SRC_DIR)/$(APP_MAIN).o

# Add TEST_MAIN to build
TEST_OBJS += $(BUILD_DIR)/$(TEST_DIR)/$(TEST_MAIN).o

# Build step for C source
# Changes in Makefile should trigger compilation too
$(BUILD_DIR)/$(SRC_DIR)/%.c.o: $(SRC_DIR)/%.c Makefile
	@[ -d $(BUILD_DIR)/$(SRC_DIR)/ ] || mkdir $(BUILD_DIR)/$(SRC_DIR)/
	$(CC) $(CFLAGS) -c $< -o $@

# TEST_MAIN should be recompile on changes in any TEST_SRCS or Makefile
$(BUILD_DIR)/$(TEST_DIR)/$(TEST_MAIN).o: $(TEST_SRCS) Makefile
	@echo "Compile tests..."
	@[ -d $(BUILD_DIR)/$(TEST_DIR)/ ] || mkdir $(BUILD_DIR)/$(TEST_DIR)/
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $(TEST_DIR)/$(TEST_MAIN) -o $@

# Build the game
compile: $(OBJS)
	@echo "Build application..."
	$(CC) $(OBJS) -o $(BUILD_DIR)/$(APP_EXEC) 

# Build tests
test: $(TEST_OBJS)
	@echo "Build and run tests..."
	$(CC) $(TEST_OBJS) -o $(BUILD_DIR)/$(TEST_EXEC) 
	$(BUILD_DIR)/$(TEST_EXEC)

run: compile
	$(BUILD_DIR)/$(APP_EXEC)


clean:
	rm -r $(BUILD_DIR)

.PHONY: clean compile test run
