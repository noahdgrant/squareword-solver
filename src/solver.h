#ifndef SOLVER_H
#define SOLVER_H

#define WORD_LENGTH 6       // +1 space for null terminator
#define WORD_COUNT 14855    // number of words in wordle-words.txt
#define NUM_ROWS 5          // number of rows in the game board

int solver(char board[NUM_ROWS][WORD_LENGTH], char unplaced[NUM_ROWS][WORD_LENGTH],
          char solution[NUM_ROWS][WORD_LENGTH], char words[WORD_COUNT][WORD_LENGTH]);

#endif
