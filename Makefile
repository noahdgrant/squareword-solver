# Variable declaration
CC=gcc
CFLAGS=-g -Wall -lpthread
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

all: $(TARGET)

# Create binary files and object files
$(TARGET): $(OBJS)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

# Test build
tests:
	./tests/run_tests.sh

# Run the main executable
run: $(TARGET)
	$(TARGET)

# Run only the solver
solver: $(TARGET)
	$(TARGET) -s

# Run only the minimum solution finder
minimum: $(TARGET)
	$(TARGET) -m

# Additional build commands
clean:
	$(RM) -r $(BIN)/* $(OBJ)/*

.PHONY: all tests run solver minimum clean
