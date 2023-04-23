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
	$(CC) $(DEBUG) -g -o $(BUILD)/$(APP) $(SRC)/app.c

run:
	@$(BUILD)/$(APP)

test-compile: compile
	$(CC) -g -I$(SRC) -o $(BUILD)/tests $(TEST)/laby_test.c

test: test-compile
	@$(BUILD)/tests

build: compile test
