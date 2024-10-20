#include <stdio.h>
#include <string.h>
#include <time.h>

#include "logger.h"
#include "solver.h"

// TODO: find all possible solutions because there might be more than one
// depending on the starting information

static unsigned long m_iteration_count = 0; // The number of times the solve() function is called

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
static int is_valid_word(char word[], char words[MAX_WORD_COUNT][WORD_LENGTH], int word_count) {
    for (int i = 0; i < word_count; i++) {
        if (strcmp(word, words[i]) == 0) {
            return 1; // Word is valid
        }
    }
    return 0; // Word is not valid
}

// Function to check if all columns with complete words are valid words
static int validate_columns(char grid[GRID_SIZE][GRID_SIZE], char words[MAX_WORD_COUNT][WORD_LENGTH],
                            int word_count) {
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
            if (!is_valid_word(word, words, word_count)) {
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
            logger(DEBUG, __func__, "'%s' does not fit in row %d - known don't match", word, row + 1);
            return 0; // Characters don't match
        }
    }

    // Check if all known unplaced characters are in the word
    for (int col = 0; col < GRID_SIZE; col++) {
        if (unplaced[row][col] == '.') {
            continue; // Unknown character
        }

        if (!strchr(word, unplaced[row][col])) {
            logger(DEBUG, __func__, "'%s' does not fit in row %d - unplaced don't match", word, row + 1);
            return 0; // Characters is not in the word
        }
    }

    logger(DEBUG, __func__, "'%s' fits in row %d", word, row + 1);
    return 1; // Word fits in row
}

// Function to place a word in the grid
void place_word(char grid[GRID_SIZE][GRID_SIZE], char word[], int row) {
    logger(DEBUG, __func__, "Placing '%s' in row %d", word, row + 1);
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[row][i] = word[i];
    }

    return;
}

// Function to remove a word in the grid
void remove_word(char board[GRID_SIZE][GRID_SIZE], char solution[GRID_SIZE][GRID_SIZE], int row) {
    char placeholder = '.';

    logger(DEBUG, __func__, "Removing the word in row %d", row + 1);
    for (int i = 0; i < GRID_SIZE; i++) {
        if (board[row][i] == placeholder) {
            solution[row][i] = placeholder;
        }
    }

    return;
}

// Function to check if a word is in the grid
static int is_in_grid(char grid[GRID_SIZE][GRID_SIZE], char word[]) {
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

static int solve(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE], 
                 char solution[GRID_SIZE][GRID_SIZE], char words[MAX_WORD_COUNT][WORD_LENGTH],
                 int word_count, int row) {
    if (is_grid_full(solution)) {
        return 1; // Puzzle solved
    }

    for (int index = 0; index < word_count; index++) {
        if (is_in_grid(solution, words[index])) {
            // cannot repeat words
            continue;
        }

        m_iteration_count++;
        if (m_iteration_count % 1000000000 == 0) {
            logger(INFO, __func__, "Processed %lu iterations", m_iteration_count);
            logger(INFO, __func__, "Current solution...");
            print_current_solution(solution, unplaced);
        }

        logger(DEBUG, __func__, "Trying '%s'", words[index]);

        if (fits_in_row(solution, unplaced, words[index], row)) {
            place_word(solution, words[index], row);
            if (validate_columns(solution, words, word_count)) {
                //print_current_solution(solution, unplaced);
                if (solve(board, unplaced, solution, words, word_count, row + 1)) {
                    return 1;
                }
            }
            remove_word(board, solution, row);
            //print_current_solution(solution, unplaced);
        }
    }

    return 0; // No solution found
}

// Function to filter out words containing unused letters
int filter_words(char words[MAX_WORD_COUNT][WORD_LENGTH], char valid_words[MAX_WORD_COUNT][WORD_LENGTH],
                 char unused_letters[], int unused_length) {
    int valid_word_count = 0;
    int valid = 1;

    for (int i = 0; i < MAX_WORD_COUNT; i++) {
        valid = 1;
        for (int u = 0; u < unused_length; u++) {
            if (strchr(words[i], unused_letters[u])) {
                valid = 0;
                break;
            }
        }
        if (valid) {
            strcpy(valid_words[valid_word_count++], words[i]);
        }
    }

    return valid_word_count; // Return the number of valid words
}

// Function to solve the squareword
int solver(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE],
           char unused[], int unused_length, char solution[GRID_SIZE][GRID_SIZE],
           char words[MAX_WORD_COUNT][WORD_LENGTH]) {
    int err_code = 0;
    clock_t start, end;
    double cpu_time_used;
    int hours, minutes, seconds;
    char valid_words[MAX_WORD_COUNT][WORD_LENGTH];
    int valid_word_count = 0;

    logger(INFO, __func__, "Starting solver");

    logger(INFO, __func__, "Number of starting words: %d", MAX_WORD_COUNT);
    valid_word_count = filter_words(words, valid_words, unused, unused_length);
    logger(INFO, __func__, "Number of valid words: %d", valid_word_count);

    // Copy beginning board state to solution
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            solution[i][j] = board[i][j];
        }
    }

    logger(INFO, __func__, "Starting board...");
    print_current_solution(solution, unplaced);

    start = clock();
    err_code = solve(board, unplaced, solution, valid_words, valid_word_count, 0);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    hours = (int) (cpu_time_used / 3600);
    minutes = (int) ((cpu_time_used - (hours * 3600)) / 60);
    seconds = (int) (cpu_time_used - (hours * 3600) - (minutes * 60));

    logger(INFO, __func__, "Number of iterations: %lu", m_iteration_count);
    logger(INFO, __func__, "Execution time: %02d:%02d:%02d", hours, minutes, seconds);

    if (err_code == 1) {
        logger(INFO, __func__, "Solution found");
        err_code = 0;
    } else {
        logger(ERROR, __func__, "Solution not found");
        err_code = 1;
    }

    logger(INFO, __func__, "Finished solver");
    return err_code;
}
