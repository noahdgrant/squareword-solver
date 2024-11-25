#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/solver.h"

void test_filter_words() {
    int word_count = 10;
    char words[MAX_WORD_COUNT][WORD_LENGTH] = {
        "await", "swail", "naled", "naked", "smile", "shale", "swine", "slide", "shade", "shame"
    };
    char result_words[MAX_WORD_COUNT][WORD_LENGTH] = {
        "swail", "naked", "smile", "swine"
    };
    int result = 1;


    char columns[GRID_SIZE][GRID_SIZE] = {
        {'s', 'i', 'l', '.', '.'},
        {'m', 'a', 'c', 'w', '.'},
        {'t', 'n', 'e', '.', '.'},
        {'s', 'i', 'n', 'e', '.'},
        {'h', 'o', 'd', '.', '.'}
    };

    char filtered_words[MAX_WORD_COUNT][WORD_LENGTH];
    int filtered_count = filter_possible_words(words, word_count, columns, filtered_words);

    assert(4 == filtered_count);
    for (int i = 0; i < filtered_count; i++) {
        result = strncmp(result_words[i], filtered_words[i], GRID_SIZE);
        assert (0 == result);
    }
}

int main() {
    test_filter_words();

    printf("All tests passed\n");
    return 0;
}
