#include <string.h>

#include "logger.h"
#include "solver.h"

int solver(char board[NUM_ROWS][WORD_LENGTH], char unplaced[NUM_ROWS][WORD_LENGTH],
          char solution[NUM_ROWS][WORD_LENGTH], char words[WORD_COUNT][WORD_LENGTH]) {
    logger(TRACE, __func__, "Enter");

    logger(INFO, __func__, "Starting solver");

    strncpy(solution[NUM_ROWS - 1], "hello", WORD_LENGTH);

    logger(INFO, __func__, "Finished solver");

    logger(TRACE, __func__, "Exit");
    return 0;
}

