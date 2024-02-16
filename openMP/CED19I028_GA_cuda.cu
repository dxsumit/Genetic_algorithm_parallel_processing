%%cu

// C++ program to create target string, starting from random string using Genetic Algorithm
 
#include "bits/stdc++.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
using namespace std;
 
// Number of individuals in each generation
#define POPULATION_SIZE 10
// #define POPULATION_SIZE 500000
int generation = 0;
 
// Valid Genes
#define GENES "`~1!2@3#4$5%6^7&8*9(0)-_+QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm[{]}|;:'\",./?< >"
#define genesSize 93

// Target string to be generated
#define TARGET "Random Generation..."
#define targetSize 20


// // Create random genes for mutation
// char mutated_genes() {
//     int len = GENES.size();
//     return GENES[rand()%len];
// }
 
// // create chromosome or string of genes
// string create_gnome() {
//     int len = TARGET.size();
//     string gnome = "";
//     for(int i=0; i<len; i++)
//         gnome += mutated_genes();
//     return gnome;
// }
 
// Class representing individual in population
class Individual {

    public:
        int fitness;
        char chromosome[targetSize];
        
        __device__ __host__ Individual(char *chromosome);  
};
 
Individual::Individual(char *chromosome) {
    // this->chromosome = chromosome;
    for (int i=0; chromosome[i] != '\0'; i++) {
        this->chromosome[i] = chromosome[i];
    }
   
    // int len = TARGET.size();
    int offspring_fitness = 0;
    for(int i=0; i<targetSize; i++) {
        if(chromosome[i] != TARGET[i]){
            offspring_fitness++;
        }
    }
    this->fitness = offspring_fitness;  
};
 
// Overloading < operator
bool compare(Individual* ind1, Individual* ind2) {
    return ind1->fitness < ind2->fitness;
}
 

__device__ float generate(curandState* globalState, int ind) {
    //int ind = threadIdx.x;
    curandState localState = globalState[ind];
    float RANDOM = curand_uniform( &localState );
    globalState[ind] = localState;
    return RANDOM;
}

__global__ void setup_kernel ( curandState * state, unsigned long seed ) {
    int id = threadIdx.x;
    curand_init ( seed, id, 0, &state[id] );
}


// curandState* globalState for generating random...
__global__ void population_kernel(Individual **population, curandState *globalState) {

    // printf("Hello..");
    
    int index = threadIdx.x + blockIdx.x * 5;       // M is number of blocks..

    int number;
    int id = threadIdx.x + blockIdx.x * blockDim.x;

    // create initial population
   
    // for(int i=0; i<POPULATION_SIZE; i++) {
    //     char gnome[targetSize];
    //     for(int j=0; j<targetSize; j++){
    //         number = generate(globalState, id)*genesSize;
    //         gnome[j] = GENES[number];
    //     }
  
    //     population[i] = new Individual(gnome);
 
    //     // for(int j=0; j<targetSize; j++){
    //     //     printf("%c", population[i]->chromosome[j]);
    //     // }
    //     // printf(" --- Fitness %d\n", population[i]->fitness);
    // }
    // // printf("\nComplete., %d", population[0]->fitness);


    if(index < POPULATION_SIZE){

        char gnome[targetSize];
        for(int j=0; j<targetSize; j++){
            number = generate(globalState, id)*genesSize;
            gnome[j] = GENES[number];
        }
  
        population[index] = new Individual(gnome);
 
        for(int j=0; j<targetSize; j++){
            printf("%c", population[index]->chromosome[j]);
        }
        printf(" --- Fitness %d\n", population[index]->fitness);

    }

    // printf("Completed...\n");



}



// Driver code
int main() {

    // cout << "\nfine";

    int size = POPULATION_SIZE * sizeof(Individual);       /////////////
    Individual **population;           // host copies        ////
    Individual **d_population;          // device copies
    
    // allocate space for host...
    population = (Individual**)malloc(size);                /////

    // allocate space for device copies
    cudaMalloc((void **)&d_population, size);


    curandState* devStates;
    cudaMalloc (&devStates, POPULATION_SIZE * sizeof(curandState));
    srand(time(0));
    /** ADD THESE TWO LINES **/
    int seed = rand();
    setup_kernel<<<2, 5>>>(devStates,seed);
    /** END ADDITION **/


    // copy data to device..
    cudaMemcpy(d_population, population, size, cudaMemcpyHostToDevice);

    // Launch kernel on GPU with N blocks
    population_kernel<<<5,2>>>(d_population, devStates);
    cudaDeviceSynchronize();

    // copy result back to host..
    cudaMemcpy(population, d_population, size, cudaMemcpyDeviceToHost);

    cout << "\nfine..\n";
    cout << "Value " << population[0]->fitness;

    // for(int index=0; index<POPULATION_SIZE; index++){

    //     cout << "Inside..\n";
    //     for(int j=0; j<targetSize; j++){
    //         cout << population[index]->chromosome[j];
    //     }
    //     cout << " --- Fitness " << population[index]->fitness << endl;

    // }

    


    // int threads;
    // bool found = false;
 
    // // create initial population
    // for(int i=0; i<POPULATION_SIZE; i++) {
    //     // string gnome = create_gnome();

    //     int len = targetSize;
    //     string gnome = "";
    //     for(int i=0; i<len; i++)
    //         gnome += GENES[rand()%genesSize];
            
        
    //     // population[i] = new Individual(gnome);
    // }
    

    // while(!found) {

    //     // sort the population in increasing order of fitness score
    //     sort(population.begin(), population.end(), compare);

    //     // if the individual having lowest fitness score ie. 0 then we know that we have reached to the target and break the loop
    //     if(population[0]->fitness <= 0){
    //         found = true;
    //         break;
    //     }

    //     // Otherwise generate new offsprings for new generation
    //     vector<Individual*> new_generation(POPULATION_SIZE);
    
    //     // Perform Elitism, that mean 10% of fittest population goes to the next generation
    //     int s = (10*POPULATION_SIZE)/100;

    //     for(int i=0; i<s; i++){
    //         new_generation[i] = population[i];
    //     }

    //     // From 50% of fittest population, Individuals will mate to produce offspring
    //     int right = (50*POPULATION_SIZE)/100;
        
    //     for(int i=s; i<POPULATION_SIZE; i++) {

    //         // int r = random_num(0, right);
    //         int r = rand()%(right+1);
    //         Individual* parent1 = population[r];

    //         // r = random_num(0, right);
    //         r = rand()%(right+1);
    //         Individual* parent2 = population[r];

    //         // chromosome for offspring
    //         string child_chromosome = "";
    //         string chromosome = parent1->chromosome;
    //         int len = chromosome.size();

    //         for(int i = 0;i<len;i++) {
    //             // random probability
    //             float p = (rand()%101)/100;
        
    //             // if prob is less than 0.45, insert gene from parent 1
    //             if(p < 0.45)
    //                 child_chromosome += chromosome[i];
    //             // if prob is between 0.45 and 0.90, insertgene from parent 2
    //             else if(p < 0.90)
    //                 child_chromosome += parent2->chromosome[i];
    //             // otherwise insert random gene(mutate), for maintaining diversity
    //             else
    //                 child_chromosome += mutated_genes();
    //         }
        
    //         // create new Individual(offspring) using generated chromosome for offspring
    //         Individual* offspring = new Individual(child_chromosome);

    //         new_generation[i] = offspring;
    //     }

    //     population = new_generation;
    //     cout<< "Generation: " << generation << "\t";
    //     cout<< "String: "<< population[0]->chromosome <<"\t";
    //     cout<< "Fitness: "<< population[0]->fitness << "\n";

    //     generation++;

    // }

    // cout<< "Generation: " << generation << "\t";
    // cout<< "String: "<< population[0]->chromosome <<"\t";
    // cout<< "Fitness: "<< population[0]->fitness << "\n";

   return 0;

}