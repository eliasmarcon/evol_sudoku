#include <ga/GASimpleGA.h>
#include <ga/GA2DArrayGenome.h>
#include <ga/std_stream.h>
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;

// Define the Sudoku puzzle size
const int SUDOKU_SIZE = 9;
const unsigned int SEED = 42;  // Choose any seed value
int POPULATION_SIZE = 2500; //2000
int MAX_GENERATIONS = 15000; //15000
int NUMBERS_TO_REMOVE = 10;
int REPLACE_COUNTER = 0;

// Global vector to store whether each cell is marked
std::vector<std::vector<int>> markedCells(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

// Global vector to store the Sudoku puzzle
std::vector<std::vector<int>> sudoku(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

/*===========================================================================================*/
/*===================================== Helper Functions ====================================*/
/*===========================================================================================*/

// Load Sudoku into a 2D vector
void loadSudoku(const std::string& sudoku_path, int puzzleNumber) {
    // Open the file
    std::ifstream file(sudoku_path);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening the file.\n";
        return;
    }

    std::string line;

    // Move to the desired puzzle number
    for (int i = 1; i <= puzzleNumber; ++i) {
        if (!std::getline(file, line)) {
            std::cerr << "Error: Not enough puzzles in the file.\n";
            exit(EXIT_FAILURE); // Exit the program if there are not enough puzzles
        }
    }

    // Read the puzzle line
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            sudoku[i][j] = line[i * SUDOKU_SIZE + j] - '0'; // Convert char to int
        }
    }

    // Close the file
    file.close();
}

// Function to print the Sudoku field
void printSudoku(const std::vector<std::vector<int>>& sudoku, std::string title) {

    cout << title << endl;

    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        if (i > 0 && i % 3 == 0) {
            std::cout << "---------------------" << std::endl;
        }

        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            if (j > 0 && j % 3 == 0) {
                std::cout << "| ";
            }

            std::cout << sudoku[i][j] << " ";
        }
        std::cout << std::endl;
    }

    cout << endl;
}

string validSudoku(const std::vector<std::vector<int>>& sudoku) {

    int row = 0, col = 0, i = 0, block = 0;
    int count[9];

    for (row = 0; row < 9; ++row) {
        memset(count, 0, 9 * sizeof(int));
        for (col = 0; col < 9; ++col) {
            if (sudoku[row][col] != '.')
                ++count[sudoku[row][col] - 1];
        }
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    for (col = 0; col < 9; ++col) {
        memset(count, 0, 9 * sizeof(int));
        for (row = 0; row < 9; ++row) {
            if (sudoku[row][col] != '.')
                ++count[sudoku[row][col] - 1];
        }
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    int block_row = 0, block_col = 0;
    for (block = 0; block < 9; ++block) {
        block_row = (block / 3) * 3, block_col = (block % 3) * 3;
        memset(count, 0, 9 * sizeof(int));
        for (row = block_row; row < (block_row + 3); ++row)
            for (col = block_col; col < (block_col + 3); ++col)
                if (sudoku[row][col] != '.')
                    ++count[sudoku[row][col] - 1];
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    return "true";
}

void setPopulationSizeAndGenerations(int& NUMBERS_TO_REMOVE){

    /*
    POPULATION_SIZE = 500 * NUMBERS_TO_REMOVE;
    MAX_GENERATIONS = 3000 * NUMBERS_TO_REMOVE;
    */
    if (NUMBERS_TO_REMOVE <= 4){

        POPULATION_SIZE = 500;
        MAX_GENERATIONS = 1500;

    } else if (NUMBERS_TO_REMOVE > 4 && NUMBERS_TO_REMOVE <= 10){

        POPULATION_SIZE = 550 * NUMBERS_TO_REMOVE;
        MAX_GENERATIONS = 3500 * NUMBERS_TO_REMOVE;

    }else if (NUMBERS_TO_REMOVE > 10 && NUMBERS_TO_REMOVE <= 15){

        POPULATION_SIZE = 650 * NUMBERS_TO_REMOVE;
        MAX_GENERATIONS = 3650 * NUMBERS_TO_REMOVE;

    }else if (NUMBERS_TO_REMOVE > 15 && NUMBERS_TO_REMOVE <= 25){
            
        POPULATION_SIZE = 750 * NUMBERS_TO_REMOVE;
        MAX_GENERATIONS = 3800 * NUMBERS_TO_REMOVE;

    }else{
            
        POPULATION_SIZE = 850 * NUMBERS_TO_REMOVE;
        MAX_GENERATIONS = 4000 * NUMBERS_TO_REMOVE;
    
    }
}

/*===========================================================================================*/
/*============================ Solving/Checking Sudoku Functions ============================*/
/*===========================================================================================*/

bool isPresentInCol(int col, int num) {
    for (int row = 0; row < SUDOKU_SIZE; row++)
        if (sudoku[row][col] == num)
            return true;
    return false;
}

bool isPresentInRow(int row, int num) {
    for (int col = 0; col < SUDOKU_SIZE; col++)
        if (sudoku[row][col] == num)
            return true;
    return false;
}

bool isPresentInBox(int boxStartRow, int boxStartCol, int num) {
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            if (sudoku[row + boxStartRow][col + boxStartCol] == num)
                return true;
    return false;
}

bool findEmptyPlace(int &row, int &col) {
    for (row = 0; row < SUDOKU_SIZE; row++)
        for (col = 0; col < SUDOKU_SIZE; col++)
            if (sudoku[row][col] == 0)
                return true;
    return false;
}

bool isValidPlace(int row, int col, int num) {
    return !isPresentInRow(row, num) && !isPresentInCol(col, num) && !isPresentInBox(row - row % 3, col - col % 3, num);
}

bool solveSudoku(int &solutionsCount, int maxSolutions) {
    int row, col;
    if (!findEmptyPlace(row, col)) {
        solutionsCount++;
        if (solutionsCount >= maxSolutions)
            return true;
        return false;
    }

    for (int num = 1; num <= 9; num++) {
        if (isValidPlace(row, col, num)) {
            sudoku[row][col] = num;
            if (solveSudoku(solutionsCount, maxSolutions))
                return true;
            sudoku[row][col] = 0;
        }
    }
    return false;
}

bool hasUniqueSolution() {
    int solutionsCount = 0;
    int maxSolutions = 2; // Set the desired number of solutions

    solveSudoku(solutionsCount, maxSolutions);

    if (solutionsCount == 1){
        cout << "Unique solution found!" << endl;
        return true;
    }
    else if (solutionsCount > 1){
        cout << "Multiple solutions found!" << endl;
        return false;
    }
    else{
        cout << "No solution found" << endl;
        return false;
    }
}

// Function to create a deep copy of a 2D vector
template<typename T>
std::vector<std::vector<T>> copyVector(const std::vector<std::vector<T>>& source) {
    std::vector<std::vector<T>> destination;
    for (const auto& row : source) {
        destination.push_back(row);
    }
    return destination;
}

/*===========================================================================================*/
/*======================================= GA Functions ======================================*/
/*===========================================================================================*/

// Objective function for Sudoku
float objective(GAGenome& g) {

    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    int fitness = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE;

    // Check each row for duplicates
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            for (int k = j + 1; k < SUDOKU_SIZE; k++) {
                if (genome.gene(i, j) == genome.gene(i, k)) {
                    fitness--;
                }
            }
        }
    }

    // Check each column for duplicates
    for (int j = 0; j < SUDOKU_SIZE; j++) {
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int k = i + 1; k < SUDOKU_SIZE; k++) {
                if (genome.gene(i, j) == genome.gene(k, j)) {
                    fitness--;
                }
            }
        }
    }

    // Check each 3x3 subgrid for duplicates
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int row = blockRow * 3 + i;
                    int col = blockCol * 3 + j;
                    for (int k = i + 1; k < 3; k++) {
                        for (int l = 0; l < 3; l++) {
                            int nextRow = blockRow * 3 + k;
                            int nextCol = blockCol * 3 + l;
                            if (genome.gene(row, col) == genome.gene(nextRow, nextCol)) {
                                fitness--;
                            }
                        }
                    }
                }
            }
        }
    }

    return (float)fitness;
}

// Define the initializer function
void initializer(GAGenome& g) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, SUDOKU_SIZE - 1);


    if (REPLACE_COUNTER < 1) {

        // Create deep copies
        auto initialSudoku = copyVector(sudoku);
        auto initialMarkedCells = copyVector(markedCells);
        
        for (int k = 0; k < NUMBERS_TO_REMOVE; k++) {
            int i = dist(rng);
            int j = dist(rng);

            if (markedCells[i][j] == 0) { // cell not marked
                sudoku[i][j] = 0;
                markedCells[i][j] = 1;
            } else {
                k--;  // Try again if the cell is already empty
            }
        }

        while (!hasUniqueSolution()) {
            // Adjust numbers to remove and try again
            NUMBERS_TO_REMOVE--;
            std::cout << "Adjusting numbers to remove: " << NUMBERS_TO_REMOVE << std::endl;

            sudoku = initialSudoku;
            markedCells = initialMarkedCells;

            // Repeat the random number removal process
            for (int k = 0; k < NUMBERS_TO_REMOVE; k++) {
                int i = dist(rng);
                int j = dist(rng);

                if (markedCells[i][j] == 0) { // cell not marked
                    sudoku[i][j] = 0;
                    markedCells[i][j] = 1;
                } else {
                    k--;  // Try again if the cell is already empty
                }
            }
        }

        // set the initial sudoku with marked values to a random number
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if(markedCells[i][j] == 1){
                    sudoku[i][j] = (std::rand() % 9) + 1; // initialize field value with a random number 1-9
                }
            }
        }

        ++REPLACE_COUNTER;
        printSudoku(markedCells, "\nChanged cells:");
        printSudoku(sudoku, "Beginning Sudoku Example:");

    }

    // replace values for other instances
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            if(markedCells[i][j] == 1){
                sudoku[i][j] = (std::rand() % 9) + 1; // initialize field value with a random number 1-9
            }
        }
    }

    // Initializer part
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    
    // Set genes in the genome and mark indices with value 0
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            genome.gene(i, j, sudoku[i][j]);
        }
    }
}

// Define the mutation function
int mutator(GAGenome& g, float p) {

    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    int nMutations = 0;

    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            // Check if the cell is marked and the mutation probability is met
            if (markedCells[i][j] == 1 && GAFlipCoin(p)) {
                // Find another marked cell to swap values with
                int x, y;
                do {
                    x = rand() % SUDOKU_SIZE;
                    y = rand() % SUDOKU_SIZE;
                } while (markedCells[x][y] != 1 || (x == i && y == j));

                // Swap values of marked cells
                int temp = genome.gene(i, j);
                genome.gene(i, j, genome.gene(x, y));
                genome.gene(x, y, temp);

                nMutations++;
            }
        }
    }

    return nMutations;
}

// Define the crossover function
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (markedCells[i][j] == 1 && GAFlipCoin(0.5)) {
                    child1.gene(i, j, parent2.gene(i, j));
                    child2.gene(i, j, parent1.gene(i, j));
                } else {
                    child1.gene(i, j, parent1.gene(i, j));
                    child2.gene(i, j, parent2.gene(i, j));
                }
            }
        }

        return 2;
    } else if (c1) {
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;

        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (markedCells[i][j] == 1 && GAFlipCoin(0.5)) {
                    child.gene(i, j, parent2.gene(i, j));
                } else {
                    child.gene(i, j, parent1.gene(i, j));
                }
            }
        }

        return 1;
    } else {
        return 0;
    }
}

/*===========================================================================================*/
/*====================================== Main Function ======================================*/
/*===========================================================================================*/
int main(int argc, char* argv[]) {

    int sudoku_number = atoi(argv[1]);
    NUMBERS_TO_REMOVE = atoi(argv[2]);
    const std::string sudoku_path = argv[3];

    setPopulationSizeAndGenerations(NUMBERS_TO_REMOVE);

    cout << "Sudoku number: " << sudoku_number << endl;
    cout << "Numbers to remove: " << NUMBERS_TO_REMOVE << endl;
    cout << "Population size: " << POPULATION_SIZE << endl;
    cout << "Max generations: " << MAX_GENERATIONS << endl;
    cout << endl;

    // Define the Sudoku puzzle
    loadSudoku(sudoku_path, sudoku_number);
    printSudoku(sudoku, "Pre-Solved Sudoku Field Number " + to_string(sudoku_number) + ":");

    // Create the initial population
    GA2DArrayGenome<int> genome(SUDOKU_SIZE, SUDOKU_SIZE, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    // Create the genetic algorithm
    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.nGenerations(MAX_GENERATIONS);
    ga.pMutation(0.05);
    ga.pCrossover(0.9);

    // Evolve the population
    ga.evolve();

    // Print the best solution
    GA2DArrayGenome<int>& bestSolution = (GA2DArrayGenome<int>&)ga.statistics().bestIndividual();
    
    // Initialize a Sudoku field with zeros
    std::vector<std::vector<int>> solvedSudoku(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    for (int row = 0; row < SUDOKU_SIZE; row++) {
        for (int col = 0; col < SUDOKU_SIZE; col++) {
            solvedSudoku[row][col] = bestSolution.gene(row, col);
        }
    }

    printSudoku(solvedSudoku, "Finished Sudoku Field:");

    string solvedValid = validSudoku(solvedSudoku);
    cout << "Answer to solved Sudoku valid? --> " << solvedValid << endl;

    return 0;
}



