APP_EXEC := labyrinth
TEST_EXEC := run_tests

CC = gcc
CFLAGS = -g # Turn on debug info

BUILD_DIR := ./build
SRC_DIR := ./src
TEST_DIR := ./test

APP_MAIN := app.c
TEST_MAIN := all_tests.c

# Find all the C files we want to compile, except src witn main function
SRCS := $(shell find $(SRC_DIR) -name '*.c' -and -not -name $(APP_MAIN))
# All tests should be included to the single test file
TEST_SRCS := $(TEST_DIR)/$(TEST_MAIN).c

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
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Build the game
compile: $(OBJS)
	$(CC) $(OBJS) -o $(BUILD_DIR)/$(APP_EXEC) 

# Build tests
test: $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(BUILD_DIR)/$(TEST_EXEC) 
	$(BUILD_DIR)/$(TEST_EXEC)

run: compile
	$(BUILD_DIR)/$(APP_EXEC)


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
