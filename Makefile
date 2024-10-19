# Variable declaration
CC=gcc
CFLAGS=-g -Wall
SRC=src
OBJ=obj
BIN=bin
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

TARGET=$(BIN)/main

# Debug build
all:$(TARGET)

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

# Additional build commands
clean:
	$(RM) -r $(BIN)/* $(OBJ)/*
