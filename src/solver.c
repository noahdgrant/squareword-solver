#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "solver.h"

static long int m_iteration_count = 0; // The number of times the solve() function is called

// Prints the current state of the solution[][] array
static void print_current_solution(char solution[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE]) {
    logger(DEBUG, __func__, "Current solution...");
    // TODO: figure out how to only print this for DEBUG log level
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            fprintf(stderr, "%c", solution[row][col]);
        }
        fprintf(stderr, " [");
        for (int col = 0; col < GRID_SIZE; col++) {
            if (unplaced[row][col] != '.') {
                fprintf(stderr, "%c", unplaced[row][col]);
            }
        }
        fprintf(stderr, "]");
        fprintf(stderr, "\n");
    }

}

static int is_grid_full(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (grid[row][col] == '.') {
                return 0; // Grid not full
            }
        }
    }
    return 1; // Grid full
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

// Function to check if a word is in the list of valid words
int is_valid_word(char word[], char words[WORD_COUNT][WORD_LENGTH]) {
    for (int i = 0; i < WORD_COUNT; i++) {
        if (strcmp(word, words[i]) == 0) {
            return 1; // Word is valid
        }
    }
    return 0; // Word is not valid
}

// Function to check if all columns with complete words are valid words
int validate_columns(char grid[GRID_SIZE][GRID_SIZE], char words[WORD_COUNT][WORD_LENGTH]) {
    for (int col = 0; col < GRID_SIZE; col++) {
        char word[WORD_LENGTH] = {0}; // Buffer to store the word
        word[WORD_LENGTH - 1] = '\0';
        int word_length = 0;

        for (int row = 0; row < GRID_SIZE; row++) {
            if (grid[row][col] != '.') {
                word[word_length++] = grid[row][col];
            } else {
                break; // Stop if an incomplete word is found
            }
        }

        if (word_length == GRID_SIZE) { // Check only complete words
            if (!is_valid_word(word, words)) {
                logger(DEBUG, __func__, "Invalid word '%s' in column %d", word, col + 1);
                return 0; // Invalid word found
            }
        }
    }
    return 1; // All complete words are valid
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
    // 2. If the word is placed in the row, all completed words are valid words
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

    logger(DEBUG, __func__, "'%s' fits in column %d", word, col + 1);
    return 1;
}

// Function to place a word in the grid
void place_word(char grid[GRID_SIZE][GRID_SIZE], char word[], int row,
                int col, int horizontal) {
    if (horizontal) {
        logger(DEBUG, __func__, "Placing '%s' in row %d", word, row + 1);
        for (int i = 0; i < GRID_SIZE; i++) {
            grid[row][i] = word[i];
        }
    } else {
        logger(DEBUG, __func__, "Placing '%s' in column %d", word, col + 1);
        for (int i = 0; i < GRID_SIZE; i++) {
            grid[i][col] = word[i];
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

// Function to check if a word is in the grid
int is_in_grid(char grid[GRID_SIZE][GRID_SIZE], char word[]) {
    int word_length = strlen(word);

    // Check horizontally
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col <= GRID_SIZE - word_length; col++) {
            int match = 1;
            for (int k = 0; k < word_length; k++) {
                if (grid[row][col + k] != word[k]) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                return 1; // Word found horizontally
            }
        }
    }

    // Check vertically
    for (int col = 0; col < GRID_SIZE; col++) {
        for (int row = 0; row <= GRID_SIZE - word_length; row++) {
            int match = 1;
            for (int k = 0; k < word_length; k++) {
                if (grid[row + k][col] != word[k]) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                return 1; // Word found vertically
            }
        }
    }

    return 0; // Word not found
}

int solve(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE], 
          char solution[GRID_SIZE][GRID_SIZE], char words[WORD_COUNT][WORD_LENGTH]) {
    if (is_grid_full(solution)) {
        return 1; // Puzzle solved
    }
    
    for (int index = 0; index < WORD_COUNT; index++) {
        m_iteration_count++;
        if (m_iteration_count % 100000000 == 0) {
            logger(INFO, __func__, "Processed %d iterations", m_iteration_count);
            logger(INFO, __func__, "Current solution...");
            print_current_solution(solution, unplaced);
        }

        if (is_in_grid(solution, words[index])) {
            // cannot repeat words
            continue;
        }

        logger(DEBUG, __func__, "Trying '%s'", words[index]);

        for (int row = 0; row < GRID_SIZE; row++) {
            if (fits_in_row(solution, unplaced, words[index], row)) {
                // rename so I don't have extra arg
                place_word(solution, words[index], row, 0, 1);
                if (validate_columns(solution, words)) {
                    print_current_solution(solution, unplaced);
                    if (solve(board, unplaced, solution, words)) {
                        return 1;
                    }
                }
                remove_word(board, solution, words[index], row, 0, 1);
                print_current_solution(solution, unplaced);
            }
        }
        //for (int col = 0; col < GRID_SIZE; col++) {
        //    if (fits_in_col(solution, words[index], col)) {
        //        // rename so I don't have extra arg
        //        place_word(solution, words[index], 0, col, 0);
        //        print_current_solution(solution, unplaced);
        //        if (solve(board, unplaced, solution, words)) {
        //            return 1;
        //        }
        //        remove_word(board, solution, words[index], 0, col, 0);
        //        print_current_solution(solution, unplaced);
        //    }
        //}
    }

    return 0; // No solution found
}

// Function to solve the squareword
int solver(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE],
           char solution[GRID_SIZE][GRID_SIZE], char words[WORD_COUNT][WORD_LENGTH]) {
    int err_code = 0;

    logger(TRACE, __func__, "Enter");
    logger(INFO, __func__, "Starting solver");
    
    // Copy beginning board state to solution
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            solution[i][j] = board[i][j];
        }
    }

    logger(INFO, __func__, "Starting board...");
    print_current_solution(solution, unplaced);

    err_code = solve(board, unplaced, solution, words);
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
