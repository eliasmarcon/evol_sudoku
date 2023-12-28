#include <ga/GASimpleGA.h>
#include <ga/GA2DArrayGenome.h>
#include <ga/std_stream.h>
#include <iostream>
#include <random>
#include <set>

using namespace std;

//removing numbers 
//https://puzzling.stackexchange.com/questions/142/removing-numbers-from-a-full-sudoku-puzzle-to-create-one-with-a-unique-solution

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


/*
int puzzle[SUDOKU_SIZE][SUDOKU_SIZE] = {
    {1, 2, 5, 8, 9, 7, 4, 3, 6},
    {3, 4, 8, 1, 2, 6, 9, 5, 7},
    {6, 7, 9, 3, 5, 4, 8, 1, 2},
    {7, 6, 4, 2, 3, 9, 1, 8, 5},
    {5, 8, 3, 7, 4, 1, 2, 6, 9},
    {9, 1, 2, 5, 6, 8, 7, 4, 3},
    {8, 3, 6, 9, 1, 2, 5, 7, 4},
    {2, 5, 1, 4, 7, 3, 6, 9, 8},
    {4, 9, 7, 6, 8, 5, 3, 2, 1}
};

// Function to convert a 2D array to a vector of vectors
std::vector<std::vector<int>> convertToVector(const int puzzle[SUDOKU_SIZE][SUDOKU_SIZE]) {
    std::vector<std::vector<int>> sudoku(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            sudoku[i][j] = puzzle[i][j];
        }
    }

    return sudoku;
}

*/

// Load Sudoku into a 2D vector
void loadSudoku(std::vector<std::vector<int>>& sudoku, int puzzleNumber) {
    // Open the file
    std::ifstream file("sudoku_solutions.txt");

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening the file.\n";
        return;
    }

    std::string line;

    // Move to the desired puzzle number
    while (std::getline(file, line) && puzzleNumber > 1) {
        --puzzleNumber;
    }

    //std::cout << line << std::endl;

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


// Objective function for Sudoku
float objective(GAGenome& g) {

    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    int fitness = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE; //int fitness = (CHESSBOARD_SIZE * (CHESSBOARD_SIZE - 1)) / 2;

    /*
    // Check for default value
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            if (genome.gene(i, j) == 0) {
                fitness--;
            }
        }
    }
    */

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
    
    //std::vector<std::vector<int>> sudoku = convertToVector(puzzle);

    /*
    string valid = validSudoku(sudoku);
    cout << "Answer to valid Sudoku: " << valid << endl;
    */

    // remove random numbers
    //std::mt19937 rng(SEED);  // Mersenne Twister random number generator
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, SUDOKU_SIZE - 1);

    if (REPLACE_COUNTER < 1)
    {
        for (int k = 0; k < NUMBERS_TO_REMOVE; k++) {
            int i = dist(rng);
            int j = dist(rng);
            
            if (markedCells[i][j] == 0) { // cell not marked
                sudoku[i][j] = (std::rand() % 9) + 1; // initialize field value with a random number 1-9
                markedCells[i][j] = 1;
            } else {
                k--;  // Try again if the cell is already empty
            }
        }

        ++REPLACE_COUNTER;
        printSudoku(markedCells, "\nChanged cells:");
    }

    // replace values for other instances
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            if(markedCells[i][j] == 1){
                sudoku[i][j] = (std::rand() % 9) + 1; // initialize field value with a random number 1-9
            }
        }
    }

    /*
    cout << "Vector:" << endl;
    // Display the content of the vector (optional)
    for (const auto& row : sudoku) {
        for (int value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

    cout << endl;
    */


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
            if (GAFlipCoin(p) && markedCells[i][j] == 1) {
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

bool getRandomBool() {
    return rand() % 2 == 0;
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
                if (markedCells[i][j] == 1 && getRandomBool()) {
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
                if (markedCells[i][j] == 1 && getRandomBool()) {
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

void setPopulationSizeAndGenerations(int& NUMBERS_TO_REMOVE){

    if (NUMBERS_TO_REMOVE < 5){

        POPULATION_SIZE = 1000;
        MAX_GENERATIONS = 10000;

    } else if (NUMBERS_TO_REMOVE > 5 && NUMBERS_TO_REMOVE < 10){

        POPULATION_SIZE = 2500;
        MAX_GENERATIONS = 15000;

    }else if (NUMBERS_TO_REMOVE > 10 && NUMBERS_TO_REMOVE < 15){

        POPULATION_SIZE = 5000;
        MAX_GENERATIONS = 30000;

    }else if (NUMBERS_TO_REMOVE > 15 && NUMBERS_TO_REMOVE < 20){
            
        POPULATION_SIZE = 10000;
        MAX_GENERATIONS = 50000;

    }else{
            
        POPULATION_SIZE = 20000;
        MAX_GENERATIONS = 1000000;
    }
}

int main(int argc, char* argv[]) {

    int sudoku_number = atoi(argv[1]);
    NUMBERS_TO_REMOVE = atoi(argv[2]);

    setPopulationSizeAndGenerations(NUMBERS_TO_REMOVE);

    cout << "\nSudoku number: " << sudoku_number << endl;
    cout << "Numbers to remove: " << NUMBERS_TO_REMOVE << endl;
    cout << "Population size: " << POPULATION_SIZE << endl;
    cout << "Max generations: " << MAX_GENERATIONS << endl;

    // Define the Sudoku puzzle
    loadSudoku(sudoku, sudoku_number);
    string valid = validSudoku(sudoku);
    cout << "\nAnswer to initial loaded Sudoku valid? --> " << valid << endl;
    //printSudoku(sudoku);

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

    printSudoku(solvedSudoku, "\nSudoku Field");

    string solvedValid = validSudoku(solvedSudoku);
    cout << "Answer to solved Sudoku valid? --> " << solvedValid << endl;

    return 0;
}



