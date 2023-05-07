.DEFAULT_GOAL = build

APP = labyrinth

CC := gcc
SRC := src
TEST := test
BUILD := build

clean:
	rm -rf $(BUILD)/

compile: clean
	mkdir $(BUILD)
	$(CC) $(DEBUG) -g -Wall -o $(BUILD)/$(APP) \
		$(SRC)/u8.c \
		$(SRC)/term.c \
		$(SRC)/laby.c \
		$(SRC)/game.c \
		$(SRC)/app.c

run: compile
	@$(BUILD)/$(APP)

test-compile: compile
	$(CC) -g -I$(SRC) -o $(BUILD)/tests \
		$(SRC)/u8.c 		\
		$(SRC)/laby.c    	\
		$(TEST)/tests.c

test: test-compile
	@$(BUILD)/tests

build: test
