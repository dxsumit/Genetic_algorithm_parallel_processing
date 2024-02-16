// C++ program to create target string, starting from random string using Genetic Algorithm
 
#include "iostream"
#include "vector"
#include "omp.h"
#include "time.h"
#include "algorithm"
using namespace std;
 
// Number of individuals in each generation
// #define POPULATION_SIZE 1000
#define POPULATION_SIZE 500000
int generation = 0;
 
// Valid Genes
const string GENES = "`~1!2@3#4$5%6^7&8*9(0)-_+QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm[{]}|;:'\",./?< >";

// Target string to be generated
const string TARGET = "Random Generation...";
 
// Function to generate random numbers in given range
// int random_num(int start, int end) {
//     int range = (end-start)+1;
//     int random_int = start+(rand()%range);
//     return random_int;
// }
 
// Create random genes for mutation
char mutated_genes() {
    int len = GENES.size();
    return GENES[rand()%len];
}
 
// create chromosome or string of genes
string create_gnome() {
    int len = TARGET.size();
    string gnome = "";
    for(int i=0; i<len; i++)
        gnome += mutated_genes();
    return gnome;
}
 
// Class representing individual in population
class Individual {

    public:
        string chromosome;
        int fitness;
        
        Individual(string chromosome);  
        // Individual* mate(Individual* parent2);
        // int cal_fitness();
};
 
Individual::Individual(string chromosome) {
    this->chromosome = chromosome;
    // this->fitness = cal_fitness();

    int len = TARGET.size();
    int offspring_fitness = 0;
    // #pragma omp parallel for shared(TARGET, chromosome) reduction(+:fitness)
    for(int i=0; i<len; i++) {
        if(chromosome[i] != TARGET[i]){
            offspring_fitness++;
        }
    }

    this->fitness = offspring_fitness;
    
};
 
// Perform mating and produce new offspring
// Individual* Individual::mate(Individual* par2) {
//     // chromosome for offspring
//     string child_chromosome = "";
//     int len = chromosome.size();

//     for(int i = 0;i<len;i++) {
//         // random probability
//         float p = (rand()%101)/100;
 
//         // if prob is less than 0.45, insert gene from parent 1
//         if(p < 0.45)
//             child_chromosome += chromosome[i];
//         // if prob is between 0.45 and 0.90, insertgene from parent 2
//         else if(p < 0.90)
//             child_chromosome += par2->chromosome[i];
//         // otherwise insert random gene(mutate), for maintaining diversity
//         else
//             child_chromosome += mutated_genes();
//     }
 
//     // create new Individual(offspring) using generated chromosome for offspring
//     return new Individual(child_chromosome);

// };
 
 
// Calculate fitness score, it is the number of
// characters in string which differ from target string.
// int Individual::cal_fitness() {

//     int len = TARGET.size();
//     int fitness = 0;
//     // #pragma omp parallel for shared(TARGET, chromosome) reduction(+:fitness)
//     for(int i=0; i<len; i++) {
//         if(chromosome[i] != TARGET[i]){
//             fitness++;
//         }
//     }
//     return fitness;   
// };
 
// Overloading < operator
bool compare(Individual* ind1, Individual* ind2) {
    return ind1->fitness < ind2->fitness;
}
 
// Driver code
int main() {

    double start_time, end_time;
    int threads;
    start_time = omp_get_wtime();
    srand(time(0));
 
    vector<Individual*> population(POPULATION_SIZE);
    bool found = false;
 
    // create initial population
    
    #pragma omp for 
        for(int i=0; i<POPULATION_SIZE; i++) {
            string gnome = create_gnome();
            population[i] = new Individual(gnome);
        }

    // #pragma omp barrier

    // #pragma omp parallel 
    // {
    while(!found) {

        // sort the population in increasing order of fitness score
        sort(population.begin(), population.end(), compare);

        // if the individual having lowest fitness score ie. 0 then we know that we have reached to the target and break the loop
        if(population[0]->fitness <= 0){
            found = true;
            break;
        }

        // Otherwise generate new offsprings for new generation
        vector<Individual*> new_generation(POPULATION_SIZE);
        // #pragma omp parallel 
        // {

            // Perform Elitism, that mean 10% of fittest population goes to the next generation
            int s = (10*POPULATION_SIZE)/100;

            #pragma omp for
                for(int i=0; i<s; i++){
                    new_generation[i] = population[i];
                }

            #pragma omp barrier 

            // From 50% of fittest population, Individuals will mate to produce offspring
            int right = (50*POPULATION_SIZE)/100;
            
            #pragma omp for
                for(int i=s; i<POPULATION_SIZE; i++) {

                    // int r = random_num(0, right);
                    int r = rand()%(right+1);
                    Individual* parent1 = population[r];

                    // r = random_num(0, right);
                    r = rand()%(right+1);
                    Individual* parent2 = population[r];

                    // chromosome for offspring
                    string child_chromosome = "";
                    string chromosome = parent1->chromosome;
                    int len = chromosome.size();

                    for(int i = 0;i<len;i++) {
                        // random probability
                        float p = (rand()%101)/100;
                
                        // if prob is less than 0.45, insert gene from parent 1
                        if(p < 0.45)
                            child_chromosome += chromosome[i];
                        // if prob is between 0.45 and 0.90, insertgene from parent 2
                        else if(p < 0.90)
                            child_chromosome += parent2->chromosome[i];
                        // otherwise insert random gene(mutate), for maintaining diversity
                        else
                            child_chromosome += mutated_genes();
                    }
                
                    // create new Individual(offspring) using generated chromosome for offspring
                    Individual* offspring = new Individual(child_chromosome);

                    new_generation[i] = offspring;

                }

            population = new_generation;
            // cout<< "Generation: " << generation << "\t";
            // cout<< "String: "<< population[0]->chromosome <<"\t";
            // cout<< "Fitness: "<< population[0]->fitness << "\n";
    
            generation++;

        #pragma omp barrier 
        // }
    }
    // }

    end_time = omp_get_wtime();
    threads = omp_get_max_threads();
    cout << threads << " " << end_time-start_time << "\n";

    // cout<< "Generation: " << generation << "\t";
    // cout<< "String: "<< population[0]->chromosome <<"\t";
    // cout<< "Fitness: "<< population[0]->fitness << "\n";

    // end_time = omp_get_wtime();
    // cout << end_time-start_time;

}

