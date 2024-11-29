# Squareword Solver

This project is a solver for the [Squareword](https://squareword.org/) game. The
solver uses a backtracking algorithm to find all possible solutions given a
starting point.

The solver can also find a combination of words to solve the puzzle in the minimum
number of guesses.

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

An `input.json` is created by the `setup.sh` script.

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
    "unused_letters": ["."],
    "columns": [
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."]
    ],
}
```

#### Solver

To setup the solver open `input.json`. A blank space is represented by a `'.'`.
In `game_board`, place the **green** letters as they appear on the game board.
In `unplaced_letters`, place the **yellow** letters for each row (the order does
not matter). In `unused_letters`, place any **gray** letters (if there are no
gray letters, leave it empty by putting a `'.'`). Once this is done, you can
build and run the solver.

#### Minimum Solution Finder

To setup the minimum solution finder open `input.json`. A blank space is
represented by a `'.'`. In `columns`, place the missing letters for each column
in each row of the array.

##### Example

If the solution is...

```
tames
atone
prude
aired
sands
```

And your current board is...

```
.....
a..ne
.ru.e
a....
.....
```

Then you are missing...

```
tames
.to..
p..d.
.ired
sands
```

So the `columns` array should look like...

```json
{
    "columns": [
        ["t", ".", "p", ".", "s"],
        ["a", "t", ".", "i", "a"],
        ["m", "o", ".", "r", "n"],
        ["e", ".", "d", "e", "d"],
        ["s", ".", ".", "d", "s"]
    ]
}
```

## Modes

This solver has two modes that can solver the puzzle in different ways or can be
used together.

### Solver

This mode finds all possible solutions to the puzzle from a given starting point.

#### Processes

By default the program creates 8 subprocesses to parallelize finding solutions.
Each process takes the next word in the word list and tries it in the first row.
If the word fits in the first row, the subprocess will try all the words in the
entire word list for rows 2-5. To change the number of subprocesses spawned, change
`NUM_PROCESSES` in `solver.h`.

### Minimum Solution Finder

This mode finds a combination of words to solve the puzzle in as few guesses as
possible. If the puzzle needs 5 guesses to solve, the solution finder will tell
you there is no minimum solution possible.

## Build

Build the project with...

```bash
$ make
```

Once you build the solver, you do not need to build it again unless you make
changes to it. To solve a new Squareword, simply update `input.json` to reflect
the new board state.

## Run

Run the solver and minimum solution finder with...

```bash
$ make run
```

This will find all possible solutions from the given starting board and the combination
of words to solve each solution in as few guesses as possible. Only the solver
portion of the `input.json` file needs to be filled out for this.

Run just the solver with...

```bash
$ make solver
```

This will find all possible solutions from the given starting board. It will not
try to find the combination of words to solve they puzzle in as few guesses as
possible. Only the solver portion of the `input.json` file needs to be filled
out for this.

Run just the minimum solution finder with...

```bash
$ make minimum
```

This will find the combination of words to solve the puzzle in as few guesses as
possible for a specific solution. It does not try to find all possible solutions.
Only the `columns` portion of the `input.json` needs to be filled out for this.

## Test

Run the tests with...

```bash
$ make tests
```

## Clean

Clean old build files with...

```bash
$ make clean
```

## Example Boards with Intended Solutions

### Solver Example 1

The solver part of the `input.json` should look like this...

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

fiats
unfit
stole
erode
doter
```

### Solver Example 2

The solver part of the `input.json` should look like this...

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

```
shams
hazel
elude
aerie
freak

slams
hazel
exude
aerie
freak

spams
hazel
elude
aerie
freak
```

### Minimum Solution Finder Example 1

The desired solution is...

```
tames
atone
prude
aired
sands
```

And your current board is...

```
.....
a..ne
.ru.e
a....
.....
```

Then your are missing...

```
tames
.to..
p..d.
.ired
sands
```

The `columns` in the `input.json` should look like this...

```json
{
    "columns": [
        ["t", ".", "p", ".", "s"],
        ["a", "t", ".", "i", "a"],
        ["m", "o", ".", "r", "n"],
        ["e", ".", "d", "e", "d"],
        ["s", ".", ".", "d", "s"]
    ]
}
```

The minimum solution found is...

```
tames
tardy
pines
stood
```
