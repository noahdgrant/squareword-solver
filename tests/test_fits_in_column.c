#include <assert.h>
#include <stdio.h>

#include "../src/solver.h"

void test_fits_in_col_pass() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'a', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_col(solution, "apple", 0);
    assert(1 == result);
}

void test_fits_in_col_fail() {
    char solution[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', 'x', '.', '.', '.'}
    };

    // Test cases
    int result = fits_in_col(solution, "apple", 1);
    assert(0 == result);
}

int main() {
    test_fits_in_col_pass();
    test_fits_in_col_fail();

    printf("All tests passed\n");
    return 0;
}
