#include <assert.h>
#include <stdio.h>

#include "../src/solver.h"

void test_place_word_row() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    char result[GRID_SIZE][GRID_SIZE] = {
        {'a', 'p', 'p', 'l', 'e'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    place_word(solution, "apple", 0, 0, 1);
    assert(1 == compare_arrays(result, solution));
}

void test_place_word_col() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    char result[GRID_SIZE][GRID_SIZE] = {
        {'a', '.', '.', '.', '.'},
        {'p', '.', '.', '.', '.'},
        {'p', '.', '.', '.', '.'},
        {'l', '.', '.', '.', '.'},
        {'e', '.', '.', '.', '.'}
    };

    // Test cases
    place_word(solution, "apple", 0, 0, 0);
    assert(1 == compare_arrays(result, solution));
}

int main() {
    test_place_word_row();
    test_place_word_col();

    printf("All tests passed\n");
    return 0;
}
