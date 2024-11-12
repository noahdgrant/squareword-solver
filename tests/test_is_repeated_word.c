#include <assert.h>
#include <stdio.h>

#include "../src/solver.h"

void test_is_repeated_word_empty_board() {
    char board[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char word[] = "aloha";

    // Test cases
    int result = is_repeated_word(board, word, 0);
    assert(0 == result);
}

void test_is_repeated_word_row_current_row() {
    char board[GRID_SIZE][GRID_SIZE] = {
        {'a', 'l', 'o', 'h', 'a'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char word[] = "aloha";

    // Test cases
    int result = is_repeated_word(board, word, 0);
    assert(0 == result);
}

void test_is_repeated_word_row_past_row() {
    char board[GRID_SIZE][GRID_SIZE] = {
        {'a', 'l', 'o', 'h', 'a'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char word[] = "aloha";

    // Test cases
    int result = is_repeated_word(board, word, 1);
    assert(1 == result);
}

void test_is_repeated_word_row_future_row() {
    char board[GRID_SIZE][GRID_SIZE] = {
        {'.', '.', '.', '.', '.'},
        {'a', 'l', 'o', 'h', 'a'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };
    char word[] = "aloha";

    // Test cases
    int result = is_repeated_word(board, word, 0);
    assert(1 == result);
}

void test_is_repeated_word_column() {
    char board[GRID_SIZE][GRID_SIZE] = {
        {'a', '.', '.', '.', '.'},
        {'l', '.', '.', '.', '.'},
        {'o', '.', '.', '.', '.'},
        {'h', '.', '.', '.', '.'},
        {'a', '.', '.', '.', '.'}
    };
    char word[] = "aloha";

    // Test cases
    int result = is_repeated_word(board, word, 0);
    assert(1 == result);
}

int main() {
    test_is_repeated_word_empty_board();
    test_is_repeated_word_row_current_row();
    test_is_repeated_word_row_past_row();
    test_is_repeated_word_row_future_row();
    test_is_repeated_word_column();

    printf("All tests passed\n");
    return 0;
}
