.DEFAULT_GOAL = build

APP = labyrinth

CC := gcc
SRC := src
TEST := test
BUILD := build

check-fmt:
	clang-format --dry-run --Werror **/*.c **/*.h

fmt:
	clang-format -i **/*.c **/*.h

clean:
	rm -rf $(BUILD)/

compile: clean
	mkdir $(BUILD)
	$(CC) $(DEBUG) -g -Wall -o $(BUILD)/$(APP) \
		$(SRC)/u8.c \
		$(SRC)/art.c \
		$(SRC)/term.c \
		$(SRC)/laby.c \
		$(SRC)/game.c \
		$(SRC)/app.c

run: compile
	@$(BUILD)/$(APP)

test-compile: compile
	$(CC) -g -I$(SRC) -o $(BUILD)/all_tests \
		$(SRC)/u8.c 		\
		$(SRC)/art.c \
		$(SRC)/laby.c  	\
		$(SRC)/term.c  	\
		$(TEST)/all_tests.c

test: test-compile
	@$(BUILD)/all_tests

build: test
