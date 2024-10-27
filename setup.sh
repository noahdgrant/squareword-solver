#!/bin/bash

# Installed requirements
sudo apt install build-essential

# Setup input file
echo '{
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
}' > "input.json"
