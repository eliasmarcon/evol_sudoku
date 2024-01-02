#include <ga/GA1DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/std_stream.h>
#include <ctime>
#include <cstdlib>
#include <iostream>

using namespace std;

const int CHESSBOARD_SIZE = 8;
const int POPULATION_SIZE = 500;
const int MAX_GENERATIONS = 2000;

// Objective function
float objective(GAGenome& g) {
    GA1DArrayGenome<int>& genome = (GA1DArrayGenome<int>&)g;
    int fitness = (CHESSBOARD_SIZE * (CHESSBOARD_SIZE - 1)) / 2;

    for (int i = 0; i < CHESSBOARD_SIZE; i++) {
        for (int j = i + 1; j < CHESSBOARD_SIZE; j++) {
            if (genome.gene(i) == genome.gene(j) ||
                (i - genome.gene(i) == j - genome.gene(j)) ||
                (i + genome.gene(i) == j + genome.gene(j))) {
                fitness--;
            }
        }
    }

    return (float)fitness;
}

// Initializer
void initializer(GAGenome& g) {
    GA1DArrayGenome<int>& genome = (GA1DArrayGenome<int>&)g;

    for (int i = 0; i < CHESSBOARD_SIZE; i++) {
        genome.gene(i, i);
    }

    for (int i = 0; i < CHESSBOARD_SIZE; i++) {
        int j = rand() % CHESSBOARD_SIZE;
        int temp = genome.gene(i);
        genome.gene(i, genome.gene(j));
        genome.gene(j, temp);
    }
}

// Mutator
int mutator(GAGenome& g, float p) {
    GA1DArrayGenome<int>& genome = (GA1DArrayGenome<int>&)g;

    int nMutations = 0;
    for (int i = 0; i < CHESSBOARD_SIZE; i++) {
        if (GAFlipCoin(p)) {
            int j = rand() % CHESSBOARD_SIZE;
            int temp = genome.gene(i);
            genome.gene(i, genome.gene(j));
            genome.gene(j, temp);
            nMutations++;
        }
    }

    return nMutations;
}

// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA1DArrayGenome<int>& parent1 = (GA1DArrayGenome<int>&)p1;
    GA1DArrayGenome<int>& parent2 = (GA1DArrayGenome<int>&)p2;

    if (c1 && c2) {
        GA1DArrayGenome<int>& child1 = (GA1DArrayGenome<int>&)*c1;
        GA1DArrayGenome<int>& child2 = (GA1DArrayGenome<int>&)*c2;

        int cut = rand() % CHESSBOARD_SIZE;

        for (int i = 0; i < cut; i++) {
            child1.gene(i, parent1.gene(i));
            child2.gene(i, parent2.gene(i));
        }

        for (int i = cut; i < CHESSBOARD_SIZE; i++) {
            child1.gene(i, parent2.gene(i));
            child2.gene(i, parent1.gene(i));
        }

        return 2;
    } else if (c1) {
        GA1DArrayGenome<int>& child = (GA1DArrayGenome<int>&)*c1;
        int cut = rand() % CHESSBOARD_SIZE;

        for (int i = 0; i < cut; i++) {
            child.gene(i, parent1.gene(i));
        }

        for (int i = cut; i < CHESSBOARD_SIZE; i++) {
            child.gene(i, parent2.gene(i));
        }

        return 1;
    } else {
        return 0;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    GA1DArrayGenome<int> genome(CHESSBOARD_SIZE, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.nGenerations(MAX_GENERATIONS);
    ga.pMutation(0.05);
    ga.pCrossover(0.9);
    ga.evolve();

    const GA1DArrayGenome<int>& bestGenome = (GA1DArrayGenome<int>&)ga.statistics().bestIndividual();
    cout << "Best solution found: ";
    for (int i = 0; i < CHESSBOARD_SIZE; i++) {
        cout << bestGenome.gene(i) + 1;
    }
    cout << endl;

    return 0;
}