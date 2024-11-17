#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "logger.h"
#include "list.h"
#include "set.h"
#include "solver.h"

typedef struct {
    int solution_count;
    char solutions[MAX_SOLUTION_COUNT][GRID_SIZE][GRID_SIZE];
    unsigned long iterations;
    int next_word_index;
    pthread_mutex_t mutex;
} shared_data_t;
shared_data_t* m_shared_data;

unsigned long m_iterations = 0;

// Prints the current state of the solution[][] array
static void print_current_solution(char solution[GRID_SIZE][GRID_SIZE],
                                   char unplaced[GRID_SIZE][GRID_SIZE]) {
    logger(DEBUG, "[%d] Current solution...", getpid());
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

static void print_missing_letters(char board[GRID_SIZE][GRID_SIZE],
                                  char solution[GRID_SIZE][GRID_SIZE]) {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (board[row][col] == '.') {
                fprintf(stderr, "%c", solution[row][col]);
            } else {
                fprintf(stderr, ".");
            }
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

// Checks if the grid has any empty slots
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

        // Check only complete words
        if (word_length == GRID_SIZE) {
            if (!is_valid_word(word, words, word_count)) {
                logger(DEBUG, "[%] Invalid word '%s' in column %d",
                       getpid(), word, col + 1);
                return 0; // Invalid word found
            }
        }
    }
    return 1; // All complete words are valid
}

// Function to check if a word would be repeated in the grid
int is_repeated_word(char grid[GRID_SIZE][GRID_SIZE], char word[], int row) {
    int word_length = 0;
    char column_word[WORD_LENGTH] = {0};
    column_word[WORD_LENGTH - 1] = '\0';

    // Check rows
    for (int i = 0; i < GRID_SIZE; i++) {
        if (strncmp(grid[i], word, GRID_SIZE) == 0) {
            // Match found
            if (i != row) {
                // Repeat word
                return 1;
            }
        }
    }

    // Check columns
    for (int col = 0; col < GRID_SIZE; col++) {
        word_length = 0;

        for (int row = 0; row < GRID_SIZE; row++) {
            if (grid[row][col] != '.') {
                column_word[word_length++] = grid[row][col];
            } else {
                break; // Stop if an incomplete word is found
            }
        }

        // Check only complete words
        if (word_length == GRID_SIZE) {
            if (strncmp(column_word, word, GRID_SIZE) == 0) {
                // Repeat word
                return 1;
            }
        }
    }

    return 0; // Word not repeated
}

// Function to filter out words containing unused letters
static int filter_words(char words[MAX_WORD_COUNT][WORD_LENGTH], char valid_words[MAX_WORD_COUNT][WORD_LENGTH],
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
    // 2. The frequency of each letter in the word is >= the number in the solution
    // and the unplaced letters combined

    int letter_freq[NUM_LETTERS] = {0};
    int word_letter_freq[NUM_LETTERS] = {0};

    // Check if known characters match
    for (int col = 0; col < GRID_SIZE; col++) {
        if (solution[row][col] == '.') {
            continue; // Unknown character
        }

        if (solution[row][col] != word[col]) {
            logger(DEBUG, "[%d] '%s' does not fit in row %d - known characters don't match",
                   getpid(), word, row + 1);
            return 0; // Characters don't match
        }
    }

    // Check character frequency
    // Count occurance of each letter in solution
    for (int i = 0; i < GRID_SIZE; i++) {
        if (solution[row][i] != '.') {
            letter_freq[solution[row][i] - 'a']++;
        }
    }

    // Add unplaced letters
    for (int i = 0; i < GRID_SIZE; i++) {
        if (unplaced[row][i] != '.') {
            letter_freq[unplaced[row][i] - 'a']++;
        }
    }

    // Count occurance of each letter in word
    for (int i = 0; i < GRID_SIZE; i++) {
        word_letter_freq[word[i] - 'a']++;
    }

    // Check that the word has at least as many of each letter
    for (int i = 0; i < NUM_LETTERS; i++) {
        if (word_letter_freq[i] < letter_freq[i]) {
            logger(DEBUG, "[%d] '%s' does not fit in row %d - letter frequency",
                   getpid(), word, row + 1);
            return 0; // Word doesn't fit
        }
    }

    logger(DEBUG, "[%d] '%s' fits in row %d", getpid(), word, row + 1);
    return 1; // Word fits in row
}

// Function to place a word in the grid
void place_word(char grid[GRID_SIZE][GRID_SIZE], char word[], int row) {
    logger(DEBUG, "[%d] Placing '%s' in row %d", getpid(), word, row + 1);
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[row][i] = word[i];
    }

    return;
}

// Function to remove a word in the grid
void remove_word(char board[GRID_SIZE][GRID_SIZE], char solution[GRID_SIZE][GRID_SIZE], int row) {
    char placeholder = '.';

    logger(DEBUG, "[%d] Removing the word in row %d", getpid(), row + 1);
    for (int i = 0; i < GRID_SIZE; i++) {
        if (board[row][i] == placeholder) {
            solution[row][i] = placeholder;
        }
    }

    return;
}

// Function that finds all possible squareword solutions given a starting position
static void solve(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE], 
                  char solution[GRID_SIZE][GRID_SIZE], char words[MAX_WORD_COUNT][WORD_LENGTH],
                  int word_count, int start_index, int end_index, int row) {
    if (is_grid_full(solution)) {
        // Solution found
        // Append to the solutions array and keep looking
        pthread_mutex_lock(&m_shared_data->mutex);
        if (m_shared_data->solution_count < MAX_SOLUTION_COUNT) {
            memcpy(m_shared_data->solutions[m_shared_data->solution_count],
                   solution, sizeof(m_shared_data->solutions[0]));
            logger(INFO, "[%d] Solution found (%d)...", getpid(),
                   m_shared_data->solution_count + 1);
            print_current_solution(solution, unplaced);
        } else {
            logger(WARNING, "[%d] Found more than %d possible solutions - %d",
                   getpid(), MAX_SOLUTION_COUNT, m_shared_data->solution_count);
        }
        m_shared_data->solution_count++;
        pthread_mutex_unlock(&m_shared_data->mutex);
        return;
    }

    for (int index = start_index; index < end_index; index++) {
        if (is_repeated_word(solution, words[index], row)) {
            // Cannot repeat words
            continue;
        }

        m_iterations++;

        if (fits_in_row(solution, unplaced, words[index], row)) {
            place_word(solution, words[index], row);
            if (validate_columns(solution, words, word_count)) {
                if (logger_get_level() == DEBUG) {
                    print_current_solution(solution, unplaced);
                }
                // For all rows after the first row, check all words
                solve(board, unplaced, solution, words, word_count, 0, word_count, row + 1);
            }
            remove_word(board, solution, row);
            if (logger_get_level() == DEBUG) {
                print_current_solution(solution, unplaced);
            }
        }
    }

    return;
}

static void generate_combinations(List columns[GRID_SIZE], int column_max,
                                  char *prefix, int depth, Set *result) {
    if (depth == GRID_SIZE) {
        set_add(result, prefix);
        return;
    }

    for (int i = 0; i < column_max; i++) {
        char new_prefix[WORD_LENGTH];
        snprintf(new_prefix, WORD_LENGTH, "%s%c", prefix, columns[depth].elements[i]);
        logger(DEBUG, "Current prefix: %s", prefix);
        generate_combinations(columns, column_max, new_prefix, depth + 1, result);
    }
}

static void find_minimum_solution(char board[GRID_SIZE][GRID_SIZE],
                           char solution[GRID_SIZE][GRID_SIZE],
                           char words[MAX_WORD_COUNT][WORD_LENGTH]) {
    logger(INFO, "The letters needed in the solution are...");
    print_missing_letters(board, solution);

    // Create missing letter column sets
    Set column_sets[GRID_SIZE] = {0};
    for (int i = 0; i < GRID_SIZE; i++) {
        set_init(&column_sets[i], CHAR_TYPE);
    }

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (board[row][col] == '.') {
                set_add(&column_sets[col], solution[row][col]);
            }
        }
    }

    logger(INFO, "Column sets...");
    for (int i = 0; i < GRID_SIZE; i++) {
        set_print(&column_sets[i]);
    }

    // Find longest set
    int max_set_length = 0;
    for (int j = 0; j < GRID_SIZE; j++) {
        if (column_sets[j].size > max_set_length) {
            max_set_length = column_sets[j].size;
        }
    }
    logger(INFO, "Theoretical minium number of words to solve: %d", max_set_length);

    // Create column lists
    List column_lists[GRID_SIZE] = {0};
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < column_sets[row].size; col++) {
            list_add(&column_lists[row], column_sets[row].elements[col].char_element);
        }
    }

    // Pad lists so they are all the same length
    for (int i = 0; i < GRID_SIZE; i++) {
        while (column_lists[i].size < max_set_length) {
            list_add(&column_lists[i], '.');
        }
    }

    logger(INFO, "Padded column lists...");
    for (int i = 0; i < GRID_SIZE; i++) {
        list_print(&column_lists[i]);
    }

    // Find all possible letter combinations and check if they are valid words
    Set all_combinations;
    set_init(&all_combinations, STRING_TYPE);
    generate_combinations(column_lists, max_set_length, "", 0, &all_combinations);

    logger(INFO, "Total combinations: %d", all_combinations.size);
    set_print(&all_combinations);

}

// Function to solve the squareword
int solver(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE],
           char unused[], int unused_length, char words[MAX_WORD_COUNT][WORD_LENGTH]) {
    int err_code = 0;
    struct timeval start, end;
    int hours, minutes, seconds;
    double elapsed_time;
    pid_t pids[NUM_PROCESSES];
    int current_word_index = 0;
    int first_row_words_checked = 0;
    char valid_words[MAX_WORD_COUNT][WORD_LENGTH];
    int valid_word_count = 0;
    char solution[GRID_SIZE][GRID_SIZE];
    int number_of_letters_in_row = 0;
    char current_char;
    int char_index;
    int unused_letters_count[NUM_LETTERS] = {0};

    // Force everything lowercase
    for (int i = 0; i < MAX_WORD_COUNT; i++) {
        for (int j = 0; j < WORD_LENGTH; j++) {
            words[i][j] = tolower(words[i][j]);
        }
    }

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            board[row][col] = tolower(board[row][col]);
        }
    }

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            unplaced[row][col] = tolower(unplaced[row][col]);
        }
    }

    for (int i = 0; i < unused_length; i++) {
        unused[i] = tolower(unused[i]);
    }

    // Validate that there are no letters in the unused array that are in the
    // in game board or unplaced letters
    for (int i = 0; i < unused_length; i++) {
        for (int row = 0; row < GRID_SIZE; row++) {
            for (int col = 0; col < GRID_SIZE; col++) {
                if (unused[i] != '.' && unused[i] == board[row][col]) {
                    logger(ERROR, "Unused letter '%c' in game board (%d, %d)",
                           unused[i], row + 1, col + 1);
                    err_code = 1;
                    return err_code;
                }
                if (unused[i] != '.' && unused[i] == unplaced[row][col]) {
                    logger(ERROR, "Unused letter '%c' in unplaced board (%d, %d)",
                           unused[i], row + 1, col + 1);
                    err_code = 1;
                    return err_code;
                }
            }
        }
    }

    // Validate that there are not more than five letters for a row between
    // the starting board and the unplaced letters
    for (int row = 0; row < GRID_SIZE; row++) {
        number_of_letters_in_row = 0;
        for (int col = 0; col < GRID_SIZE; col++) {
            if (board[row][col] != '.') {
                number_of_letters_in_row++;
            }
            if (unplaced[row][col] != '.') {
                number_of_letters_in_row++;
            }
        }
        if (number_of_letters_in_row > GRID_SIZE) {
            logger(ERROR, "Too many letters for row between game board and unused letters (%d)",
                   number_of_letters_in_row);
            err_code = 1;
            return err_code;
        }
    }

    // Validate that there are no duplicates in an unplaced row
    for (int row = 0; row < GRID_SIZE; row++) {
        int unplaced_row_letters[NUM_LETTERS] = {0};
        for (int col = 0; col < GRID_SIZE; col++) {
            current_char = unplaced[row][col];
            char_index = current_char - 'a';
            if (current_char != '.') {
                if (unplaced_row_letters[char_index]) {
                    logger(ERROR, "Duplicate letter '%c' found in unplaced row %d",
                           current_char, row + 1);
                    err_code = 1;
                    return err_code;
                }
                unplaced_row_letters[char_index] = 1;
            }
        }
    }

    // Validate there are no duplicates in the unused characters
    for (int i = 0; i < unused_length; i++) {
        current_char = unused[i];
        if (current_char != '.') {
            if (unused_letters_count[current_char - 'a']) {
                logger(ERROR, "Duplicate letter '%c' found in unused letters",
                       current_char);
                err_code = 1;
                return err_code;
            }
            unused_letters_count[current_char - 'a'] = 1;
        }
    }

    logger(INFO, "Starting solver");

    logger(INFO, "Number of starting words: %d", MAX_WORD_COUNT);
    valid_word_count = filter_words(words, valid_words, unused, unused_length);
    logger(INFO, "Number of valid words: %d", valid_word_count);
    logger(DEBUG, "List of valid words...");
    if (logger_get_level() == DEBUG) {
        for (int i = 0; i < valid_word_count; i++) {
            fprintf(stderr, "%s\n", valid_words[i]);
        }
    }

    if (unused_length == 1 && unused[0] == '.') {
        logger(INFO, "No unused letters");
    } else {
        logger(INFO, "Unused letter(s) (%d): %s", unused_length, unused);
    }

    // Copy beginning board state to solution
    memcpy(solution, board, GRID_SIZE * GRID_SIZE * sizeof(char));

    logger(INFO, "Starting board...");
    print_current_solution(board, unplaced);

    // Setup shared memory
    m_shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (m_shared_data == MAP_FAILED) {
        logger(ERROR, "Failed to setup shared memory");
        err_code = 1;
        return err_code;
    }

    m_shared_data->solution_count = 0;
    m_shared_data->iterations = 0;
    m_shared_data->next_word_index = 0;
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    if (pthread_mutex_init(&m_shared_data->mutex, &mutex_attr) != 0) {
        logger(ERROR, "Failed to initialize the mutex"); 
        err_code = 1;
        return err_code; 
    }

    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            logger(ERROR, "Fork failed");
            err_code = 1;
            return err_code;
        } else if (pids[i] == 0) {
            // Child process
            logger(INFO, "[%d] Starting", getpid());

            while (1) {
                pthread_mutex_lock(&m_shared_data->mutex);
                current_word_index = m_shared_data->next_word_index;
                m_shared_data->next_word_index++;
                pthread_mutex_unlock(&m_shared_data->mutex);

                if (current_word_index < valid_word_count) {
                    logger(DEBUG, "[%d] Checking '%s' - current index is %d",
                           getpid(), valid_words[current_word_index], current_word_index);
                    solve(board, unplaced, solution, valid_words, valid_word_count,
                          current_word_index, current_word_index + 1, 0);
                    first_row_words_checked++;
                } else {
                    // All first row words have been tried
                    break;
                }
            }

            pthread_mutex_lock(&m_shared_data->mutex);
            m_shared_data->iterations += m_iterations;
            pthread_mutex_unlock(&m_shared_data->mutex);

            logger(INFO, "[%d] Finished - checked %d first row words in %lu iterations",
                   getpid(), first_row_words_checked, m_iterations);

            exit(0);
        } else {
            // Parent process
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        waitpid(pids[i], NULL, 0);
    }

    gettimeofday(&end, NULL);

    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    hours = (int) (elapsed_time / 3600);
    minutes = (int) ((elapsed_time - (hours * 3600)) / 60);
    seconds = (int) (elapsed_time - (hours * 3600) - (minutes * 60));
    logger(INFO, "Execution time: %02d:%02d:%02d", hours, minutes, seconds);

    pthread_mutex_lock(&m_shared_data->mutex);
    logger(INFO, "Total number of iterations: %lu", m_shared_data->iterations);

    if (m_shared_data->solution_count == 0) {
        logger(ERROR, "Did not find any solutions");
        err_code = 1;
    } else {
        logger(INFO, "Found %d solution(s)...", m_shared_data->solution_count);
    }

    // Print solutions
    for (int i = 0; i < m_shared_data->solution_count; i++) {
        for (int row = 0; row < GRID_SIZE; row++) {
            for (int col = 0; col < GRID_SIZE; col++) {
                fprintf(stderr, "%c", m_shared_data->solutions[i][row][col]);
            }
            fprintf(stderr, "\n");
        }
            fprintf(stderr, "\n");
    }
    pthread_mutex_unlock(&m_shared_data->mutex);

    logger(INFO, "Finding minium number of words needed for each solution...");
    for (int i = 0; i < m_shared_data->solution_count; i++) {
        find_minimum_solution(board, m_shared_data->solutions[i], words);
    }

    // Clean up
    pthread_mutex_destroy(&m_shared_data->mutex); 
    munmap(m_shared_data, sizeof(shared_data_t));

    logger(INFO, "Done");
    return err_code;
}
