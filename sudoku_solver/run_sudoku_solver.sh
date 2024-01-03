#!/bin/bash

# set variables
sudokuSolutions="../sudoku_solutions.txt"
executable="./out/sudoku_solver"
cpp_file="./src/sudoku_solver.cpp"
outputfile="./sudoku_solver_output.txt"

# read the amount of lines in the file
lineCount=$(wc -l < "$sudokuSolutions")

# Set default values if arguments are not provided
sudokuNumber="$lineCount"
numbersToRemove=(1 + RANDOM % 81)

# Check for the number of arguments
if [ "$#" -eq 1 ]; then
    # Only numbersToRemove is provided, use default value for numbersToRemove
    numbersToRemove=$1
elif [ "$#" -eq 2 ]; then
    # Both numbersToRemove and sudokuNumber are provided
    sudokuNumber=$1
    numbersToRemove=$2
elif [ "$#" -gt 2 ]; then
    # Too many arguments
    echo "Usage: $0 [<sudokuNumber>] [<numbersToRemove>]"
    echo "Usage: $0 [<numbersToRemove>] --> default sudokuNumber is last sudoku in $sudokuSolutions"
    echo "Usage: $0 --> default sudokuNumber is last sudoku in $sudokuSolutions and default numbersToRemove is random between 1 and 81"
    exit 1
fi

if [ "$sudokuNumber" -lt 1 ] || [ "$sudokuNumber" -gt "$lineCount" ]; then
    echo "The sudoku number has to be between 1 and $lineCount"
    exit 1
fi

# check if the numbers to remove are valid has to be between 1 and 15
if [ "$numbersToRemove" -lt 2 ] || [ "$numbersToRemove" -gt 81 ]; then
    echo "The numbers to remove has to be between 2 and 81"
    exit 1
fi

# Compile the C++ file
g++ -o $executable $cpp_file -lga -fpermissive 

#clear the output file
> $outputfile

# Running the executable
echo "Running the sudoku solver..."
$executable $sudokuNumber $numbersToRemove $sudokuSolutions >> $outputfile
echo "Results saved into $outputfile."
