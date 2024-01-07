# Sudoku Generator and Solver

### Notes
- Ensure that the GAlib library is correctly installed and linked during compilation.
- Adjust the program parameters (e.g., population size, generation size) in the source code as needed.
- The program utilizes a genetic algorithm to create and solve Sudoku solutions. The genetic operations include initialization, mutation, and crossover as well as selection.

---

### Disclaimer
This program is provided as-is and may require adjustments based on your specific environment and requirements.
Feel free to explore and modify the source code for experimentation and customization.

**Also the ideal usage would be to just run the `run_creator_solver.sh` script to generate a new sudoku field and also solve the newly created sudoku field, with a specific number of values which are replaced. ! Also make sure to execute the `run_creator_solver.sh` script in the default folder**

### Folder Structure

- `./sudoku_creator`:

  - `./src/`: includes the `sudoku_creator.cpp` script for generating a valid sudoku 

  - `./out/`: includes the compiled `sudoku_creator.cpp` script

  - `run_sudoku_creator.sh`: is a fully finished bash script which compiles and runs the `sudoku_creator.cpp` and saves the output in the `sudoku_creator_output.txt`

  - `sudoku_creator_output.txt`: stores the output of the sudoku creator like time taken, best fitness, generations, the initial genome and the valid sudoku

- `./sudoku_solver`:

  - `./src/`: includes the `sudoku_solver.cpp` script for generating a solved sudoku field based on a number of fields which should be replaced 

  - `./out/`: includes the compiled `sudoku_solver.cpp` script

  - `run_sudoku_solver.sh`: is a fully finished bash script which compiles and runs the `sudoku_solver.cpp` and saves the output in the `sudoku_solver_output.txt`

  - `sudoku_solver_output.txt`: stores the output of the sudoku solver like the chosen sudoku number, number of fields to replace, population size, max generation size, the changed cells and the beginning and finished sudoku.

- `run_creator_solver.sh`: this bash script runs both bash scripts `run_sudoku_creator.sh`, and `run_sudoku_solver.sh` after another and saves the results in the mentioned txt files

- `sudoku_solutions.txt`: in this txt file **ALL** valid sudokus are saved from the `sudoku_creator.cpp` script

---

### Run Creator and Solver Bash Script

The `run_creator_solver.sh` script compiles and runs both the generator and solver.cpp files, but each .cpp file (generator, solver) can be run independently if wanted (therefore are the additional bash scripts for the generator and solver). The `run_creator_solver.sh` bash script automates the compilation and execution of the two C++ programs: a Sudoku creator and a Sudoku solver.
The script is divided into two main sections, one for the Sudoku creator and the other for the Sudoku solver.

#### Usage for the `run_creator_solver.sh`:
- The script provides usage instructions if the number of arguments is incorrect (it takes a `<sudokuNumber>` (which is the number of the sudoku which should be taken) and `<numbersToRemove>` (number of fields which are replaced). Both arguments have default values and are therefore pootinal, so they dont have to be provided for a valid run.
- The script also performs validation checks on provided arguments, such as ensuring the Sudoku number is within a valid range (between 1 and the max number of sudokus in the `sudoku_solutions.txt` file) and the number of fields to remove is between 2 and 81.

**Of coures the `run_sudoku_creator.sh` and `run_sudoku_solver.sh` scripts provide the same functionality. If you run them separately make sure to be in the correct folder (cd into sudoku_creator for the `run_sudoku_creator.sh` file and cd into sudoku_solver for the `run_sudoku_solver.sh` file)**

---

## Sudoku Generator

This C++ program generates Sudoku puzzles using a genetic algorithm implemented with the GAlib library. The genetic algorithm evolves a population of Sudoku solutions over several generations, aiming to improve the fitness of the solutions with each iteration.
It begins by generating valid 3x3 subgrids for each subgrid and marks the diagonal subgrids which are not going to change throughout the evolutions, all the subgrids below and above the diagonal can change (crossover and mutator function). 
The fixed 3x3 diagonal subgrids are used to make the generation of a valid sudoku faster.

### Requirements
- C++ compiler with support for C++11 features
- GAlib library

### Compilation
Compile the program using the following command:

```sh
g++ -o ./sudoku_creator/out/sudoku_creator ./sudoku_creator/src/sudoku_creator.cpp -lga -fpermissive -Ofast
```

### Usage

Run the compiled program using the following command:

- the `<sudoku_solutions_file.txt>` argument is the path where the `sudoku_solutions.txt` file is

```sh
./sudoku_creator/out/sudoku_creator <path_sudoku_solutions_file.txt>
```

or run the included bash script `run_sudoku_creator.sh` using the following command:

```sh
cd ./sudoku_creator
./run_sudoku_creator.sh
```

### Output
The program will output information about the evolution process, including the progress percentage, generation number, and the best fitness achieved. 
The final Sudoku solution will be displayed, and its validity will be checked. If the solution is valid and the sudoku does not appear in the `sudoku_solutions.txt` file, it will be saved to the file to store it with other solutions.

The program generates a Sudoku puzzle and outputs the following information:

- Progress Percentage: Shows the progress of the genetic algorithm's evolution process.
- Generation Number: Indicates the current generation of the genetic algorithm.
- Best Fitness: represents the fitness score of the best Sudoku solution in the current generation.
- Final Best Fitness: Displays the final fitness score of the best Sudoku solution.
- Time Taken for Evolution: Shows the elapsed time in hours, minutes, seconds, and milliseconds for the evolution process.
- Best Found Sudoku Field: Presents the final Sudoku solution.

The mentioned output will also be saved in a txt file which is at `./sudoku_creator/sudoku_creator_output.txt`

### Saving Solutions
The program appends the generated Sudoku solution to the specified output file. If the solution is already present in the file, it will not be appended again.

---

## Sudoku Solver

This C++ program solves Sudoku puzzles using a genetic algorithm implemented with the GAlib library. The genetic algorithm iteratively evolves a population of Sudoku solutions to improve their fitness and converge towards a valid solution.

### Requirements
- C++ compiler with support for C++11 features
- GAlib library

### Compilation
Compile the program using the following command:

```sh
g++ -o ./sudoku_solver/out/sudoku_solver ./sudoku_solver/src/sudoku_solver.cpp -lga -fpermissive -Ofast
```

### Usage
Run the compiled program using the following command:

- the `<sudoku_number>` argument is the number of the sudoku which should be taken from the `sudoku_solutions.txt` file
- the `<numbers_to_remove>` argument is the number of random fields which should be replaced with a random value (1-9) 
- the `<sudoku_solutions_file.txt>` argument is the path where the `sudoku_solutions.txt` file is

```sh
./sudoku_solver/out/sudoku_solver <sudoku_number> <numbers_to_remove> <path_sudoku_solutions_file.txt>
```

or 

```sh
cd ./sudoku_solver
./run_sudoku_solver.sh [<sudoku_number>] [<numbers_to_remove>]
```



### Output
The program will output information about the evolution process, including population size, maximum generations, progress, and the best fitness achieved.
The final solved Sudoku puzzle will be displayed, and its validity will be checked. If the solution is valid, the program will indicate that a unique solution was found.

The program generates the following output:

- Sudoku Number: Indicates the specific Sudoku puzzle being solved.
- Numbers to Remove: Displays the count of numbers removed from the Sudoku puzzle for solving.
- Population Size: Shows the size of the population used in the genetic algorithm.
- Max Generations: Indicates the maximum number of generations the genetic algorithm will evolve.
- Pre-Solved Sudoku Field: Displays the initial state of the Sudoku puzzle.
- Finished Sudoku Field: Presents the solved Sudoku puzzle.
- Answer Validity: Indicates whether the solved Sudoku puzzle is valid.
- The output information is saved in a txt file located at `./sudoku_solver/sudoku_solver_output.txt.`


## Authors

- Elias Marcon MSc. | [@eliasmarcon](https://github.com/eliasmarcon)
- Ing. Fabian Steiner BSc.| [@fasteiner](https://github.com/fasteiner/)
- Jan Langela Regincos BSc. | [@janpilu](https://github.com/janpilu)