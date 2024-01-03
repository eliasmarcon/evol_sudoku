#!/bin/bash

# Set the paths to your two bash scripts
script1="./sudoku_creator/run_sudoku_creator.sh"
script2="./sudoku_solver/run_sudoku_solver.sh"

# Execute the first script
echo "Executing $script1"
bash "$script1"
echo "Creator Done!"

# Check the exit status of the first script
if [ $? -eq 0 ]; then
    # If the first script was successful, execute the second script
    echo "Executing $script2"
    bash "$script2"
    echo "Solver Done!"
else
    echo "Error: $script1 failed. Skipping $script2."
fi