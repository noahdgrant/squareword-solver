#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "solver.h"

static char m_board[GRID_SIZE][GRID_SIZE];
static char m_unplaced[GRID_SIZE][GRID_SIZE];
static char m_solution[GRID_SIZE][GRID_SIZE];
static char m_words[WORD_COUNT][WORD_LENGTH];

// Prints the current state of the solution[][] array
static void print_current_solution() {
    logger(DEBUG, __func__, "Current solution...");
    // TODO: figure out how to only print this for DEBUG log level
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            fprintf(stderr, "%c", m_solution[row][col]);
        }
        fprintf(stderr, " [");
        for (int col = 0; col < GRID_SIZE; col++) {
            if (m_unplaced[row][col] != '.') {
                fprintf(stderr, "%c", m_unplaced[row][col]);
            }
        }
        fprintf(stderr, "]");
        fprintf(stderr, "\n");
    }

}

void print_grid(char grid[GRID_SIZE][GRID_SIZE]) {
    // TODO: figure out how to only print this for DEBUG log level
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            fprintf(stderr, "%c", grid[row][col]);
        }
        fprintf(stderr, "\n");
    }

}

// Function to compare two 2D arrays
int compare_arrays(char arr1[GRID_SIZE][GRID_SIZE], char arr2[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (arr1[i][j] != arr2[i][j]) {
                return 0; // Arrays are not equal
            }
        }
    }
    return 1; // Arrays are equal
}

// Function to check if a word fits in a row
int fits_in_row(char solution[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE],
                char word[], int row) {
    // For a word to fit in a row, a word must:
    // 1. Have the known letters in the same place as the game board
    // 2. The unplaced letters match the other letters in the word being checked

    // Check if known characters match
    for (int col = 0; col < GRID_SIZE; col++) {
        if (solution[row][col] == '.') {
            continue; // Unknown character
        }

        if (solution[row][col] != word[col]) {
            logger(DEBUG, __func__, "'%s' does not fit in row %d", word, row + 1);
            return 0; // Characters don't match
        }
    }

    // Check if all known unplaced characters are in the word
    for (int col = 0; col < GRID_SIZE; col++) {
        if (unplaced[row][col] == '.') {
            continue; // Unknown character
        }

        if (!strchr(word, unplaced[row][col])) {
            logger(DEBUG, __func__, "'%s' does not fit in row %d", word, row + 1);
            return 0; // Characters is not in the word
        }
    }

    logger(DEBUG, __func__, "'%s' fits in row %d", word, row + 1);
    return 1; // Word fits in row
}

// Function to check if a word fits in a column
int fits_in_col(char solution[GRID_SIZE][GRID_SIZE], char word[], int col) {
    // For a word to fit in a column, a word must:
    // 1. Have the known letters in the same place as the game board
    // There are no unplaced letters for a column

    for (int row = 0; row < GRID_SIZE; row++) {
        if (solution[row][col] == '.') {
            continue; // No information about the character, continue
        }

        if (solution[row][col] != word[row]) {
            logger(DEBUG, __func__, "'%s' does not fit in column %d", word, col + 1);
            return 0; // Characters don't match
        }
    }

    logger(DEBUG, __func__, "'%s' fits in col %d", word, col + 1);
    return 1;
}

// Function to place a word in the grid
void place_word(char solution[GRID_SIZE][GRID_SIZE], char word[], int row,
                int col, int horizontal) {
    if (horizontal) {
        logger(DEBUG, __func__, "Placing '%s' in row %d", word, row + 1);
        for (int i = 0; i < GRID_SIZE; i++) {
            solution[row][i] = word[i];
        }
    } else {
        logger(DEBUG, __func__, "Placing '%s' in column %d", word, col + 1);
        for (int i = 0; i < GRID_SIZE; i++) {
            solution[i][col] = word[i];
        }
    }

    return;
}

// Function to remove a word in the grid
void remove_word(char board[GRID_SIZE][GRID_SIZE], char solution[GRID_SIZE][GRID_SIZE],
                 char word[], int row, int col, int horizontal) {
    char placeholder = '.';

    if (horizontal) {
        logger(DEBUG, __func__, "Removing '%s' from row %d", word, row + 1);
        for (int i = 0; i < GRID_SIZE; i++) {
            if (board[row][i] == placeholder) {
                solution[row][i] = placeholder;
            }
        }
    } else {
        logger(DEBUG, __func__, "Removing '%s' from column %d", word, col + 1);
        for (int i = 0; i < GRID_SIZE; i++) {
            if (board[row + i][col] == placeholder) {
                solution[row + i][col] = placeholder;
            }
        }
    }

    return;
}

int solve(int word_index) {
    if (word_index == WORD_COUNT) {
        return 1; // Puzzle solved
    }

    logger(DEBUG, __func__, "Trying '%s'", m_words[word_index]);

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (fits_in_row(m_solution, m_unplaced, m_words[word_index], row)) {
                place_word(m_solution, m_words[word_index], row, col, 1);
                print_current_solution();
                if (solve(word_index + 1)) {
                    return 1;
                }
                remove_word(m_board, m_solution, m_words[word_index], row, col, 1);
            }
            if (fits_in_col(m_solution, m_words[word_index], col)) {
                place_word(m_solution, m_words[word_index], row, col, 1);
                print_current_solution();
                if (solve(word_index + 1)) {
                    return 1;
                }
                remove_word(m_board, m_solution, m_words[word_index], row, col, 1);
            }
        }
    }
    return 0; // No solution found
}

// Function to solve the squareword
int solver(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE],
           char solution[GRID_SIZE][GRID_SIZE], char words[WORD_COUNT][WORD_LENGTH]) {
    int err_code = 0;

    logger(TRACE, __func__, "Enter");
    logger(INFO, __func__, "Starting solver");
    
    // Initialize global variables
    // TODO: see if these can just be pointers so I don't have to copy everything
    memcpy(m_board, board, sizeof(m_board));
    memcpy(m_unplaced, unplaced, sizeof(m_unplaced));
    memcpy(m_solution, board, sizeof(m_solution)); // Start solution with current board
    memcpy(m_words, words, sizeof(m_words));   

    err_code = solve(0);
    if (err_code == 1) {
        logger(INFO, __func__, "Solution found");
        err_code = 0;
    } else {
        logger(ERROR, __func__, "Solution not found");
        err_code = 1;
    }

    logger(INFO, __func__, "Finished solver");
    logger(TRACE, __func__, "Exit");
    return err_code;
}
