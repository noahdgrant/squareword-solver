#include <assert.h>
#include <stdio.h>

#include "../src/solver.h"

void test_remove_word_row() {
    char board[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    char solution[GRID_SIZE][GRID_SIZE] = {
        {'a', 'p', 'p', 'l', 'e'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    remove_word(board, solution, 0);
    assert(1 == compare_arrays(board, solution));
}

void test_remove_word_row_leave_letters() {
    char board[GRID_SIZE][GRID_SIZE] = {
        {'a', '.', '.', 'l', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    char solution[GRID_SIZE][GRID_SIZE] = {
        {'a', 'p', 'p', 'l', 'e'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    remove_word(board, solution, 0);
    assert(1 == compare_arrays(board, solution));
}

int main() {
    test_remove_word_row();
    test_remove_word_row_leave_letters();

    printf("All tests passed\n");
    return 0;
}
