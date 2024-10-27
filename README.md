# Squareword Solver

This project is a solver for the [Squareword](https://squareword.org/) game. The
solver uses a backtracking algorithm to find all possible solutions given a
starting point.

### Word List

According to [Squareword](https://squareword.org/), the word list used for the
game is based on Yet Another Word List (YAWL) by Mendel Leo Cooper. The
`words.txt` file contains all the five letter words from YAWL.

## Setup

### System

Run the following commands to setup your system and create the necessary files...

```bash
$ chmod +x setup.sh
$ ./setup.sh
```

### input.json

To setup the solver open `input.json` (which was created by the `setup.sh` script).
A blank space is represented by a `'.'`. In `game_board`, place the **green**
letters as they appear on the game board. In `unplaced_letters`, place the
**yellow** letters for each row (the order does not matter). In `unused_letters`,
place any **gray** letters (if there are no gray letters, leave it empty by putting
a `'.'`). Once this is done, you can build and run the solver.

The blank starting `input.json` should like like the following...

```json
{
    "game_board": [
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."]
    ],
    "unplaced_letters": [
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."]
    ],
    "unused_letters": ["."]
}
```

### Processes

By default the program creates 8 subprocesses to parallelize finding solutions.
The word list is divided into 8 chunks and each process gets one chunk. The
subprocess tries each word in its list as the word to go in the first row. If the
word fits in the first row, the subprocess will try all the words in the entire
word list for rows 2-5. To change the number of subprocesses spawned, change
`NUM_PROCESSES` in `solver.h`.

## Build

Build the project with...

```bash
$ make
```

Once you build the solver, you do not need to build it again unless you make
changes to it. To solve a new Squareword, simply update `input.json` to reflect
the new board state and run `./bin/main`.

## Run

Run the binary from the project root with...

```bash
$ ./bin/main
```

## Test

Run the tests with...

```bash
$ make test
```

## Clean

Clean old build files with...

```bash
$ make clean
```

## Example Boards with Intended Solutions

### Example 1

The `input.json` should look like this...

```json
{
    "game_board": [
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "t"],
        [".", ".", ".", ".", "e"],
        [".", "r", ".", ".", "e"],
        [".", "o", ".", ".", "."]
    ],
    "unplaced_letters": [
        ["a", "i", "s", "t", "."],
        ["i", "u", "n", ".", "."],
        ["s", "o", "t", ".", "."],
        ["e", "o", ".", ".", "."],
        ["e", "t", ".", ".", "."]
    ],
    "unused_letters": ["."]
}
```

The possible solutions found are...

```
fiats
unfit
stole
erode
doted
```
and...

```
fiats
unfit
stole
erode
doter
```

### Example 2

The `input.json` should look like this...

```json
{
    "game_board": [
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", "u", ".", "e"],
        ["a", ".", ".", ".", "e"],
        [".", "r", ".", ".", "."]
    ],
    "unplaced_letters": [
        ["a", "s", "m", ".", "."],
        ["a", "e", ".", ".", "."],
        ["e", ".", ".", ".", "."],
        ["e", "i", "r", ".", "."],
        ["a", "e", ".", ".", "."]
    ],
    "unused_letters": ["t", "o", "n"]
}
```

The possible solutions found are...
