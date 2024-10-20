#!/bin/bash

# Directory containing test files
TEST_DIR=tests

# Compile and run each test file
for test_file in $TEST_DIR/*.c; do
    test_name=$(basename $test_file .c)
    gcc -g -Wall -o bin/$test_name $test_file src/solver.c src/logger.c -I src
    if [ $? -eq 0 ]; then
        echo "Running $test_name..."
        ./bin/$test_name
    else
        echo "Failed to compile $test_name"
    fi
done
