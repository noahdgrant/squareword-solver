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

    return err_code;
}

int get_word_list(char words[MAX_WORD_COUNT][WORD_LENGTH]) {
    FILE *file;
    int count = 0;
    char line[WORD_LENGTH + 1]; // +1 for \n

    logger(DEBUG, __func__, "Loading wordle words...");

    // Open the file in read mode
    file = fopen("words.txt", "r");
    if (file == NULL) {
        logger(ERROR, __func__, "Could not open file");
        return 1;
    }

    // Read each line from the file
    while (fgets(line, sizeof(line), file)) {
        line[WORD_LENGTH - 1] = '\0'; // Remove newline character
        strncpy(words[count], line, WORD_LENGTH);
        count++;
    }

    // Close the file
    fclose(file);

    logger(DEBUG, __func__, "Finished loading wordle words...");
    return 0;
}

int main(int argc, char* argv[]) {
    int err_code = 0;
    char words[MAX_WORD_COUNT][WORD_LENGTH];

    // the game board (green)
    char board[GRID_SIZE][GRID_SIZE] = {
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    };

    // the characters that go in the words but you don't know where (yellow)
    char unplaced[GRID_SIZE][GRID_SIZE] = {
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    };

    // letters that can't be in the final solution (grey)
    char unused[] = {'.'};
    int unused_length = sizeof(unused)/sizeof(unused[0]);

    err_code = parse_args(argc, argv);
    if (err_code != 0) {
        return err_code;
    }

    err_code = get_word_list(words);
    if (err_code != 0) {
        return err_code;
    }

    err_code = solver(board, unplaced, unused, unused_length, words);
    if (err_code != 0) {
        return err_code;
    }

    return 0;
}
