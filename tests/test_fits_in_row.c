#include <assert.h>
#include <stdio.h>

#include "../src/solver.h"

void test_fits_in_row_pass() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'a', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char unplaced[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_row(solution, unplaced, "apple", 0);
    assert(1 == result);
}

void test_fits_in_row_fail() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'b', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char unplaced[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_row(solution, unplaced, "apple", 1);
    assert(0 == result);
}

void test_fits_in_row_unplaced_pass() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char unplaced[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'a', 'p', 'e', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_row(solution, unplaced, "apple", 2);
    assert(1 == result);
}

void test_fits_in_row_unplaced_fail() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char unplaced[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'a', 'p', 'e', 'x', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_row(solution, unplaced, "apple", 3);
    assert(0 == result);
}

void test_fits_in_row_double_letter_pass() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'b', 'e', '.', 't', 's'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char unplaced[GRID_SIZE][GRID_SIZE] = {
        {'e', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_row(solution, unplaced, "beets", 0);
    assert(1 == result);
}

void test_fits_in_row_double_letter_fail() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'b', 'e', '.', 't', 's'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char unplaced[GRID_SIZE][GRID_SIZE] = {
        {'e', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_row(solution, unplaced, "beats", 0);
    assert(0 == result);
}

int main() {
    test_fits_in_row_pass();
    test_fits_in_row_fail();
    test_fits_in_row_unplaced_pass();
    test_fits_in_row_unplaced_fail();
    test_fits_in_row_double_letter_pass();
    test_fits_in_row_double_letter_fail();

    printf("All tests passed\n");
    return 0;
}
