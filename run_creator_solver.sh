#!/bin/bash

############################################################################################################
############################################### CREATOR ####################################################
############################################################################################################

# Set the name of the C++ source file for the Sudoku creator
cpp_creator_file="./sudoku_creator/src/sudoku_creator.cpp"
output_creator_executable="./sudoku_creator/out/sudoku_creator"
output_creator_file="./sudoku_creator/sudoku_creator_output.txt"
sudoku_solutions_file="./sudoku_solutions.txt"

# Clear the output file
> "$output_creator_file"

# Compile the C++ program for Sudoku creator
g++ -o "$output_creator_executable" "$cpp_creator_file" -lga -fpermissive -Ofast

# Check if compilation was successful for Sudoku creator
if [ $? -eq 0 ]; then
    echo "Compilation of Sudoku creator successful. Running the program..."

    # Run the compiled program for Sudoku creator and append the output to the file
    "$output_creator_executable" "$sudoku_solutions_file" >> "$output_creator_file"

    echo "Sudoku was saved into $output_creator_file and the $sudoku_solutions_file file."
else
    echo "Compilation of Sudoku creator failed. Please check for errors in your C++ code."
fi

echo ""


############################################################################################################
################################################ SOLVER ####################################################
############################################################################################################

# Check for the number of tasks and diameter arguments
if [ "$#" -gt 2 ]; then
    echo "Usage: $0 [<sudokuNumber>] [<numbersToRemove>]"
    exit 1
fi

# Set default values if arguments are not provided
sudokuNumber=${1:-1}
numbersToRemove=${2:-4}

# Set the name of the C++ source file for the Sudoku solver
cpp_solver_file="./sudoku_solver/src/sudoku_solver.cpp"
output_solver_executable="./sudoku_solver/out/sudoku_solver"
output_solver_file="./sudoku_solver/sudoku_solver_output.txt"


# read the amount of lines in the file
lineCount=$(wc -l < "$sudoku_solutions_file")

if [ "$sudokuNumber" -lt 1 ] || [ "$sudokuNumber" -gt "$lineCount" ]; then
    echo "The sudoku number has to be between 1 and $lineCount"
    exit 1
fi

# check if the numbers to remove are valid has to be between 1 and 15
if [ "$numbersToRemove" -lt 2 ] || [ "$numbersToRemove" -gt 81 ]; then
    echo "The numbers to remove has to be between 2 and 81"
    exit 1
fi

# Clear the output file
> "$output_solver_file"

# Compile the C++ program for Sudoku solver
g++ -o "$output_solver_executable" "$cpp_solver_file" -lga -fpermissive

# Check if compilation was successful for Sudoku solver
if [ $? -eq 0 ]; then
    echo "Compilation of Sudoku solver successful. Running the program..."

    # Run the compiled program for Sudoku solver and append the output to the file
    "$output_solver_executable" "$sudokuNumber" "$numbersToRemove" >> "$output_solver_file"

    echo "Sudoku solved. Results saved into $output_solver_file."
else
    echo "Compilation of Sudoku solver failed. Please check for errors in your C++ code."
fi
