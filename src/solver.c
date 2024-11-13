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
#include <stdio.h>
#include <string.h>

static void print_current_solution(char solution[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE]) {
    const int buffer_size = 1024;
    char buffer[buffer_size];
    int offset = 0;

    logger(DEBUG, __func__, "[%d] Current solution...", getpid());
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            offset += snprintf(buffer + offset, buffer_size - offset, "%c", solution[row][col]);
        }
        offset += snprintf(buffer + offset, buffer_size - offset, " [");
        for (int col = 0; col < GRID_SIZE; col++) {
            if (unplaced[row][col] != '.') {
                offset += snprintf(buffer + offset, buffer_size - offset, "%c", unplaced[row][col]);
            }
        }
        offset += snprintf(buffer + offset, buffer_size - offset, "]");
        offset += snprintf(buffer + offset, buffer_size - offset, "\n");
    }
    buffer[offset] = '\0';
    fprintf(stderr, "%s", buffer);
}

//static void print_current_solution(char solution[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE]) {
//    logger(DEBUG, __func__, "[%d] Current solution...", getpid());
//    for (int row = 0; row < GRID_SIZE; row++) {
//        for (int col = 0; col < GRID_SIZE; col++) {
//            fprintf(stderr, "%c", solution[row][col]);
//        }
//        fprintf(stderr, " [");
//        for (int col = 0; col < GRID_SIZE; col++) {
//            if (unplaced[row][col] != '.') {
//                fprintf(stderr, "%c", unplaced[row][col]);
//            }
//        }
//        fprintf(stderr, "]");
//        fprintf(stderr, "\n");
//    }
//}

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
                logger(DEBUG, __func__, "[%] Invalid word '%s' in column %d",
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
            logger(DEBUG, __func__, "[%d] '%s' does not fit in row %d - known characters don't match",
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
            logger(DEBUG, __func__, "[%d] '%s' does not fit in row %d - letter frequency",
                   getpid(), word, row + 1);
            return 0; // Word doesn't fit
        }
    }

    logger(DEBUG, __func__, "[%d] '%s' fits in row %d", getpid(), word, row + 1);
    return 1; // Word fits in row
}

// Function to place a word in the grid
void place_word(char grid[GRID_SIZE][GRID_SIZE], char word[], int row) {
    logger(DEBUG, __func__, "[%d] Placing '%s' in row %d", getpid(), word, row + 1);
    for (int i = 0; i < GRID_SIZE; i++) {
        grid[row][i] = word[i];
    }

    return;
}

// Function to remove a word in the grid
void remove_word(char board[GRID_SIZE][GRID_SIZE], char solution[GRID_SIZE][GRID_SIZE], int row) {
    char placeholder = '.';

    logger(DEBUG, __func__, "[%d] Removing the word in row %d", getpid(), row + 1);
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
            logger(INFO, __func__, "[%d] Solution found (%d)...", getpid(),
                   m_shared_data->solution_count + 1);
            print_current_solution(solution, unplaced);
        } else {
            logger(WARNING, __func__, "[%d] Found more than %d possible solutions - %d",
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
                    pthread_mutex_lock(&m_shared_data->mutex);
                    print_current_solution(solution, unplaced);
                    pthread_mutex_unlock(&m_shared_data->mutex);
                }
                // For all rows after the first row, check all words
                solve(board, unplaced, solution, words, word_count, 0, word_count, row + 1);
            }
            remove_word(board, solution, row);
            if (logger_get_level() == DEBUG) {
                pthread_mutex_lock(&m_shared_data->mutex);
                print_current_solution(solution, unplaced);
                pthread_mutex_unlock(&m_shared_data->mutex);
            }
        }
    }

    return;
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

    logger(INFO, __func__, "Starting solver");

    logger(INFO, __func__, "Number of starting words: %d", MAX_WORD_COUNT);
    valid_word_count = filter_words(words, valid_words, unused, unused_length);
    logger(INFO, __func__, "Number of valid words: %d", valid_word_count);
    logger(DEBUG, __func__, "List of valid words...");
    if (logger_get_level() == DEBUG) {
        for (int i = 0; i < valid_word_count; i++) {
            fprintf(stderr, "%s\n", valid_words[i]);
        }
    }

    logger(INFO, __func__, "Unused letter(s) (%d): %s", unused_length, unused);

    // Copy beginning board state to solution
    memcpy(solution, board, GRID_SIZE * GRID_SIZE * sizeof(char));

    logger(INFO, __func__, "Starting board...");
    print_current_solution(solution, unplaced);

    // Setup shared memory
    m_shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (m_shared_data == MAP_FAILED) {
        logger(ERROR, __func__, "Failed to setup shared memory");
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
        logger(ERROR, __func__, "Failed to initialize the mutex"); 
        err_code = 1;
        return err_code; 
    }

    gettimeofday(&start, NULL);
    // Split up word list for each process
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            logger(ERROR, __func__, "Fork failed");
            err_code = 1;
            return err_code;
        } else if (pids[i] == 0) {
            // Child process
            logger(INFO, __func__, "[%d] Starting", getpid());

            while (1) {
                pthread_mutex_lock(&m_shared_data->mutex);
                current_word_index = m_shared_data->next_word_index;
                m_shared_data->next_word_index++;
                pthread_mutex_unlock(&m_shared_data->mutex);

                if (current_word_index < valid_word_count) {
                    logger(DEBUG, __func__, "[%d] Checking '%s' - current index is %d",
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

            logger(INFO, __func__, "[%d] Finished - checked %d first row words in %lu iterations",
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
    logger(INFO, __func__, "Execution time: %02d:%02d:%02d", hours, minutes, seconds);

    pthread_mutex_lock(&m_shared_data->mutex);
    logger(INFO, __func__, "Total number of iterations: %lu", m_shared_data->iterations);

    if (m_shared_data->solution_count == 0) {
        logger(ERROR, __func__, "Did not find any solutions");
        err_code = 1;
    } else {
        logger(INFO, __func__, "Found %d solution(s)...", m_shared_data->solution_count);
    }

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

    // Clean up
    pthread_mutex_destroy(&m_shared_data->mutex); 
    munmap(m_shared_data, sizeof(shared_data_t));

    logger(INFO, __func__, "Finished solver");
    return err_code;
}
