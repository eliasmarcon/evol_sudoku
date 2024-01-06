#include <ga/GA2DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/std_stream.h>
#include <ga/GAStatistics.h>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <unordered_set>
#include <fstream>
#include <sstream>

using namespace std;

const int SUDOKU_SIZE = 9;
const int POPULATION_SIZE = 1500; //5
const int MAX_GENERATIONS = 20000; //200000

bool PRINT_COUNTER = true;

// Objective function for Sudoku
float objective(GAGenome& g) {

    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    int fitness = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE; //int fitness = (CHESSBOARD_SIZE * (CHESSBOARD_SIZE - 1)) / 2;

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

    std::default_random_engine rng(std::random_device{}());
    
    // get a random number between 1 and the size of the genome
    int random_number_swaps = rand() % (SUDOKU_SIZE * SUDOKU_SIZE) + 1;

    // for i in range of random swap two random numbers
    for (int i = 0; i < random_number_swaps; i++) {
        
        if (GAFlipCoin(p)) {
            int row1 = rand() % SUDOKU_SIZE;
            int col1 = rand() % SUDOKU_SIZE;
            int row2 = rand() % SUDOKU_SIZE;
            int col2 = rand() % SUDOKU_SIZE;

            int temp = genome.gene(row1, col1);
            genome.gene(row1, col1, genome.gene(row2, col2));
            genome.gene(row2, col2, temp);

            nMutations++;
        }
    }

    return nMutations;
}

// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;

    // merge 3x3 subgrids of parent1 and parent2
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<int> cutDistributionRow(0, SUDOKU_SIZE - 1);
    std::uniform_int_distribution<int> cutDistributionCol(0, SUDOKU_SIZE - 1);

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

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



#include <ga/GASelector.h>
class BestTwoOutOfFiveSelector : public GASelectionScheme {
public:
    GADefineIdentity("BestTwoOutOfFiveSelector", 0);

    BestTwoOutOfFiveSelector() : GASelectionScheme() {}
    virtual ~BestTwoOutOfFiveSelector() {}

    virtual GASelectionScheme* clone() const {
        return new BestTwoOutOfFiveSelector;
    }

    virtual void assign(GAPopulation& p) {
        GASelectionScheme::assign(p);
    }

    virtual void update() {
        GASelectionScheme::update();
    }

    virtual GAGenome& select() const {
        int idx[5];
        GAGenome* candidates[5];

        for (int i = 0; i < 5; ++i) {
            idx[i] = GARandomInt(0, pop->size() - 1);
            candidates[i] = &(pop->individual(idx[i]));
        }

        GAGenome* best1 = candidates[0];
        GAGenome* best2 = candidates[1];

        if (best2->score() > best1->score()) {
            swap(best1, best2);
        }

        for (int i = 2; i < 5; ++i) {
            if (candidates[i]->score() > best2->score()) {
                best2 = candidates[i];
                if (best2->score() > best1->score()) {
                    swap(best1, best2);
                }
            }
        }

        return *best2;
    }
};

// Function to save Sudoku to a file
void saveSudokuToFile(const std::vector<std::vector<int>>& sudoku, const std::string& filename) {
    std::ifstream inFile(filename);
    std::ostringstream currentSudoku;

    if (inFile.is_open()) {
        // Read the contents of the file into a stringstream
        currentSudoku << inFile.rdbuf();
        inFile.close();
    }

    // Convert the stringstream to a string
    std::string fileContents = currentSudoku.str();

    // Convert the current Sudoku to a string for comparison
    std::ostringstream currentSudokuStr;
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            currentSudokuStr << sudoku[i][j];
        }
    }

    // Check if the Sudoku solution is already present in the file
    if (fileContents.find(currentSudokuStr.str()) != std::string::npos) {
        cout << "Sudoku solution is already in the file. Not appending." << endl;
        return;
    }

    // Append the Sudoku solution to the file
    std::ofstream outFile(filename, std::ios::app);

    if (outFile.is_open()) {
        for (int i = 0; i < SUDOKU_SIZE; ++i) {
            for (int j = 0; j < SUDOKU_SIZE; ++j) {
                outFile << sudoku[i][j];
            }
        }

        outFile << "\n";
        outFile.close();

        cout << "Sudoku solution saved to " << filename << endl;
    } else {
        std::cerr << "Unable to open the file for saving Sudoku solution." << std::endl;
    }
}


int main(int argc, char* argv[]) {
    
    srand(static_cast<unsigned int>(time(nullptr)));

    // std::string solutions_path = argv[1];

    // Initialize a Sudoku field with zeros
    std::vector<std::vector<int>> sudoku(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    // Start measuring time
    auto start_time = chrono::high_resolution_clock::now();

    GA2DArrayGenome<int> genome(SUDOKU_SIZE, SUDOKU_SIZE, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.pMutation(0.7);
    ga.pCrossover(0.9);
    ga.minimaxi(GAGeneticAlgorithm::MAXIMIZE);
    ga.set(gaNnGenerations, MAX_GENERATIONS);

    BestTwoOutOfFiveSelector selector;
    ga.selector(selector);

    GA2DArrayGenome<int> bestGenome = genome;
    int initialFitness = 0;
    int bestFitness = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE;

    // Evolve and output information for each generation
    for (int generation = 1; generation <= MAX_GENERATIONS; ++generation) {
        
        ga.evolve();

        // Access statistics
        GAStatistics stats = ga.statistics();

        if (generation % 1 == 0) {
            // Output information for each generation
            std::cout << "Prozent done: " << std::ceil(static_cast<double>(generation) / MAX_GENERATIONS * 100) << "% | Generation " << generation
                    << " | Best Fitness: " << stats.bestIndividual().score() << std::endl;
        }

        // Check if the current best individual is better than the overall best
        if (stats.bestIndividual().score() > initialFitness) {
            
            initialFitness = stats.bestIndividual().score();
            bestGenome = (GA2DArrayGenome<int>&)stats.bestIndividual();
            
            if (initialFitness == bestFitness){
                break;
            }
        }

    }

    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++){
            sudoku[i][j] = bestGenome.gene(i, j);
        }
    }

    // Stop measuring time
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    // Calculate hours, minutes, and seconds
    auto hours = chrono::duration_cast<chrono::hours>(duration);
    duration -= hours;
    auto minutes = chrono::duration_cast<chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = chrono::duration_cast<chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration);

    // Print elapsed time in hours, minutes, seconds, and milliseconds
    cout << "\nTime taken for evolution: " << hours.count() << " hours, "
        << minutes.count() << " minutes, "
        << seconds.count() << " seconds, and "
        << milliseconds.count() << " milliseconds\n" << endl;

    std::cout << "Final Best Fitness: " << initialFitness << std::endl << std::endl;

    cout << "Best found Sudoku Field" << endl;
    printSudoku(sudoku);
    cout << endl;

    string valid = validSudoku(sudoku);
    cout << "Answer to valid Sudoku --> " << valid << endl;    

    // if (valid == "true") {
    //     // Save the Sudoku solution to a file
    //     saveSudokuToFile(sudoku, solutions_path); 
    // }

    return 0;
}