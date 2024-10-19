#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include "solver.h"

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [options]\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help              Display this help message and exit\n");
    fprintf(stderr, "  -l, --log <log_level>   Set the log level (TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)\n");
}

int parse_args(int argc, char* argv[]) {
    int err_code = 0;
    int opt;
    static struct option long_options[] = {
        {"log", required_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    logger(TRACE, __func__, "Enter");

    while ((opt = getopt_long(argc, argv, "l:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'l':
                logger_set_level(logger_level_from_string(optarg));
                err_code = 0;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            default:
                print_usage(argv[0]);
                err_code = 1;
                break;
        }
    }

    logger(TRACE, __func__, "Exit");
    return err_code;
}

int get_wordle_words(char words[WORD_COUNT][WORD_LENGTH]) {
    FILE *file;
    int count = 0;
    char line[WORD_LENGTH + 1]; // +1 for \n

    logger(TRACE, __func__, "Enter");
    logger(DEBUG, __func__, "Loading wordle words...");

    // Open the file in read mode
    file = fopen("wordle-words.txt", "r");
    if (file == NULL) {
        logger(ERROR, __func__, "Could not open file");
        return 1;
    }

    // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        line[WORD_LENGTH - 1] = '\0'; // Remove newline character
        logger(TRACE, __func__, "Line (%d): %s\n", count + 1, line);
        strncpy(words[count], line, WORD_LENGTH);
        count++;
    }

    // Close the file
    fclose(file);

    logger(DEBUG, __func__, "Finished loading wordle words...");
    logger(TRACE, __func__, "Exit");
    return 0;
}

int main(int argc, char* argv[]) {
    int err_code = 0;
    char wordle_words[WORD_COUNT][WORD_LENGTH];

    // the game board (green)
    char board[NUM_ROWS][WORD_LENGTH] = {
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    };

    // the characters that go in the words but you don't know where (yellow)
    char unplaced[NUM_ROWS][WORD_LENGTH] = {
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    };

    // the final solution
    char solution[NUM_ROWS][WORD_LENGTH] = {
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    {'.','.','.','.','.', '\0'},
    };

    err_code = parse_args(argc, argv);
    if (err_code != 0) {
        return err_code;
    }

    err_code = get_wordle_words(wordle_words);
    if (err_code != 0) {
        return err_code;
    }

    err_code = solver(board, unplaced, solution, wordle_words);
    if (err_code != 0) {
        return err_code;
    }

    logger(INFO, __func__, "Final solution...");
    for (int i = 0; i < NUM_ROWS; i++) {
        printf("%s\n", solution[i]);
    }

    return 0;
}

//void test() {
//    char board[NUM_ROWS][WORD_LENGTH] = {
//    {'.','.','.','.','.', '\0'},
//    {'.','.','.','.','t', '\0'},
//    {'.','.','.','.','e', '\0'},
//    {'.','r','.','.','e', '\0'},
//    {'.','o','.','.','.', '\0'},
//    };
//
//    char unplaced[NUM_ROWS][WORD_LENGTH] = {
//    {'a','i','s','t','.', '\0'},
//    {'i','u','n','.','.', '\0'},
//    {'s','o','t','.','.', '\0'},
//    {'e','o','.','.','.', '\0'},
//    {'e','t','.','.','.', '\0'},
//    };
//
//    char solution[NUM_ROWS][WORD_LENGTH] = {
//    {'.','.','.','.','.', '\0'},
//    {'.','.','.','.','.', '\0'},
//    {'.','.','.','.','.', '\0'},
//    {'.','.','.','.','.', '\0'},
//    {'.','.','.','.','.', '\0'},
//    };
//
//    char final_solution[NUM_ROWS][WORD_LENGTH] = {
//    {'f','i','a','t','s', '\0'},
//    {'u','n','f','i','t', '\0'},
//    {'s','t','o','l','e', '\0'},
//    {'e','r','o','d','e', '\0'},
//    {'d','o','t','e','d', '\0'},
//    };
//}
