#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json.h"
#include "logger.h"
#include "parser.h"
#include "solver.h"

int get_word_list(char words[MAX_WORD_COUNT][WORD_LENGTH]) {
    FILE *file;
    int count = 0;
    char line[WORD_LENGTH + 1]; // +1 for \n

    logger(DEBUG, "Loading words...");

    // Open the file in read mode
    file = fopen("words.txt", "r");
    if (file == NULL) {
        logger(ERROR, "Could not open file");
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

    logger(DEBUG, "Finished loading words...");
    return 0;
}

int main(int argc, char* argv[]) {
    Args args;
    int err_code = 0;
    char words[MAX_WORD_COUNT][WORD_LENGTH];
    char game_board[GRID_SIZE][GRID_SIZE];
    char unplaced_letters[GRID_SIZE][GRID_SIZE];
    char unused_letters[NUM_LETTERS];
    int unused_count = 0;
    char columns[GRID_SIZE][GRID_SIZE];

    args = parse_args(argc, argv);

    err_code = json_parse(game_board, unplaced_letters, unused_letters, &unused_count,
                          columns);
    if (err_code) {
        exit(err_code);
    }

    err_code = get_word_list(words);
    if (err_code) {
        exit(err_code);
    }

    err_code = setup();
    if (err_code) {
        exit(err_code);
    }

    if (args.solve == true) {
        err_code = solver(game_board, unplaced_letters, unused_letters, unused_count, words);
        if (err_code) {
            cleanup();
            exit(err_code);
        }

        if (args.find_minimum == true) {
            err_code = find_minimum_solutions(game_board, words);
            if (err_code) {
                cleanup();
                exit(err_code);
            }
        }
    } else {
        err_code = find_minimum_solution(columns, words);
        if (err_code) {
            cleanup();
            exit(err_code);
        }
    }

    cleanup();

    return 0;
}
