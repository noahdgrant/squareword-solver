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
    place_word(solution, "apple", 0);
    assert(1 == compare_arrays(result, solution));
}

int main() {
    test_place_word_row();

    printf("All tests passed\n");
    return 0;
}
