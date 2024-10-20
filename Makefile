# Variable declaration
CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
BIN=bin
TESTS=tests
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
TEST_SRCS=$(wildcard $(TESTS)/*.c)
TEST_OBJS=$(patsubst $(TESTS)/%.c, $(OBJ)/%.o, $(TEST_SRCS))

TARGET=$(BIN)/main
TEST_TARGET=$(BIN)/test_runner

# Debug build
all: $(TARGET)

# Release build
release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: $(TARGET)

# Create binary files and object files
$(TARGET): $(OBJS)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

# Test build
test:
	./tests/run_tests.sh

# Run the main executable
run: $(TARGET)
	$(TARGET)

# Additional build commands
clean:
	$(RM) -r $(BIN)/* $(OBJ)/*
