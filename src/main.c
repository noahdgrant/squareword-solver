#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json.h"
#include "logger.h"
#include "solver.h"

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [options] <filename>\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help              Display this help message and exit\n");
    fprintf(stderr, "  -l, --log <log_level>   Set the log level (TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)\n");
}

int parse_args(int argc, char* argv[], char** filename) {
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

    if (optind < argc) {
        *filename = argv[optind];
    } else {
        logger(ERROR, __func__, "Filename is required\n");
        print_usage(argv[0]);
        err_code = 1;
    }

    return err_code;
}

int get_word_list(char words[MAX_WORD_COUNT][WORD_LENGTH]) {
    FILE *file;
    int count = 0;
    char line[WORD_LENGTH + 1]; // +1 for \n

    logger(DEBUG, __func__, "Loading words...");

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

    logger(DEBUG, __func__, "Finished loading words...");
    return 0;
}

int main(int argc, char* argv[]) {
    int err_code = 0;
    char* filename = NULL;
    char words[MAX_WORD_COUNT][WORD_LENGTH];
    char game_board[GRID_SIZE][GRID_SIZE];
    char unplaced_letters[GRID_SIZE][GRID_SIZE];
    char unused_letters[NUM_LETTERS];
    int unused_count = 0;

    err_code = err_code & parse_args(argc, argv, &filename);
    err_code = err_code & json_parse(filename, game_board, unplaced_letters,
                                     unused_letters, &unused_count);
    err_code = err_code & get_word_list(words);
    err_code = err_code & solver(game_board, unplaced_letters, unused_letters,
                                 unused_count, words);
    return err_code;
}
