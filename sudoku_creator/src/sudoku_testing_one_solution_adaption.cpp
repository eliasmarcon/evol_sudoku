#include <ga/GA2DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/std_stream.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

using namespace std;

const int SUDOKU_SIZE = 9;
const int POPULATION_SIZE = 2000;
const int MAX_GENERATIONS = 25000;
bool PRINT_COUNTER = true;

// Objective function for Sudoku
float objective(GAGenome& g) {

    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    int fitness = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE; //int fitness = (CHESSBOARD_SIZE * (CHESSBOARD_SIZE - 1)) / 2;

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

// Initializer
void initializer(GAGenome& g) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    vector<int> nums;
    for (int i = 1; i <= SUDOKU_SIZE; i++) {
        nums.push_back(i);
    }

    // Shuffle the numbers to have different candidates
    std::default_random_engine rng(std::random_device{}());
    std::shuffle(nums.begin(), nums.end(), rng);

    
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        std::shuffle(nums.begin(), nums.end(), rng);

        for (int j = 0; j < SUDOKU_SIZE; j++) {
            genome.gene(i, j, nums[j]);
        }
    }
    
    if (PRINT_COUNTER){
        //show genome
        cout << "Initial Genome: " << endl;
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++){
                cout << genome.gene(i, j) << " ";
            }
            cout << endl;
        }
        cout << endl;
        PRINT_COUNTER = false;
    }
}

// Mutator
int mutator(GAGenome& g, float p) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    int nMutations = 0;
    // Switch a random amount of 3x3 subgrids
    std::default_random_engine rng(std::random_device{}());
    int mutationsToPerform = std::uniform_int_distribution<int>(1, 81)(rng);
    
    for (int mutationCount = 0; mutationCount < mutationsToPerform; mutationCount++) {
        
        std::uniform_int_distribution<int> positionDistribution(0, SUDOKU_SIZE - 1);
        
        int i1 = positionDistribution(rng);
        int j1 = positionDistribution(rng);
        int i2 = positionDistribution(rng);
        int j2 = positionDistribution(rng);

        int temp = genome.gene(i1, j1);
        genome.gene(i1, j1, genome.gene(i2, j2));
        genome.gene(i2, j2, temp);

        nMutations++;
    }

    return nMutations;
}

// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

        std::default_random_engine rng(std::random_device{}());
        std::uniform_int_distribution<int> cutDistributionRow(0, SUDOKU_SIZE - 1);
        std::uniform_int_distribution<int> cutDistributionCol(0, SUDOKU_SIZE - 1);

        int cutRow = cutDistributionRow(rng);
        int cutCol = cutDistributionCol(rng);

        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (i < cutRow || (i == cutRow && j < cutCol)) {
                    child1.gene(i, j, parent1.gene(i, j));
                    child2.gene(i, j, parent2.gene(i, j));
                } else {
                    child1.gene(i, j, parent2.gene(i, j));
                    child2.gene(i, j, parent1.gene(i, j));
                }
            }
        }

        return 2;
    } else if (c1) {
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;
        
        std::default_random_engine rng(std::random_device{}());
        std::uniform_int_distribution<int> cutDistributionRow(0, SUDOKU_SIZE - 1);
        std::uniform_int_distribution<int> cutDistributionCol(0, SUDOKU_SIZE - 1);

        int cutRow = cutDistributionRow(rng);
        int cutCol = cutDistributionCol(rng);

        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (i < cutRow || (i == cutRow && j < cutCol)) {
                    child.gene(i, j, parent1.gene(i, j));
                } else {
                    child.gene(i, j, parent2.gene(i, j));
                }
            }
        }

        return 1;
    } else {
        return 0;
    }
}

// Function to print the Sudoku field
void printSudoku(const std::vector<std::vector<int>>& sudoku) {
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

int main() {
    
    srand(static_cast<unsigned int>(time(nullptr)));

    // Initialize a Sudoku field with zeros
    std::vector<std::vector<int>> sudoku(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    GA2DArrayGenome<int> genome(SUDOKU_SIZE, SUDOKU_SIZE, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.nGenerations(MAX_GENERATIONS);
    ga.pMutation(0.05); //0.05
    ga.pCrossover(0.9); //0.9
    
    // Evolve and output information for each generation
    for (int generation = 0; generation < MAX_GENERATIONS; ++generation) {
        
        ga.evolve();

        // Access statistics
        GAStatistics stats = ga.statistics();

        if (generation % 100 == 0) {
            cout << "Generation " << generation
                << " Best Fitness: " << stats.bestIndividual().score() << endl;
        }
    }

    const GA2DArrayGenome<int>& bestGenome = (GA2DArrayGenome<int>&)ga.statistics().bestIndividual();
    
    cout << "Best solution found: " << endl;
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++){
            cout << bestGenome.gene(i, j) << " ";
            sudoku[i][j] = bestGenome.gene(i, j);
        }
        cout << endl;
    }
    cout << endl;

    cout << "Sudoku Field" << endl;
    printSudoku(sudoku);
    cout << endl;

    string valid = validSudoku(sudoku);
    cout << "Answer to valid Sudoku: " << valid << endl;    


    return 0;
}
