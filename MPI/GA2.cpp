// C++ program to create target string, starting from random string using Genetic Algorithm
 
#include "iostream"
#include <mpi.h>
#include "vector"
#include "time.h"
#include "algorithm"
using namespace std;

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2
 
// Number of individuals in each generation
#define POPULATION_SIZE 10
// #define POPULATION_SIZE 500000
int generation = 0;
 
// Valid Genes
const string GENES = "`~1!2@3#4$5%6^7&8*9(0)-_+QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm[{]}|;:'\",./?< >";

// Target string to be generated
const string TARGET = "Random Generation..";
#define targetSize 19
 
// Function to generate random numbers in given range
// int random_num(int start, int end) {
//     int range = (end-start)+1;
//     int random_int = start+(rand()%range);
//     return random_int;
// }
 
// Create random genes for mutation
// char mutated_genes() {
//     int len = GENES.size();
//     return GENES[rand()%len];
// }
 
// create chromosome or string of genes
string create_gnome() {
    int len = TARGET.size();
    string gnome = "";
    for(int i=0; i<len; i++)
        gnome += GENES[rand()%len];
    return gnome;
}
 
// // Class representing individual in population
// class Individual {

//     public:
//         string chromosome;
//         int fitness;
        
//         Individual(string chromosome);  
//         // Individual* mate(Individual* parent2);
//         // int cal_fitness();
// };
 
// Individual::Individual(string chromosome) {
//     this->chromosome = chromosome;
//     // this->fitness = cal_fitness();

    // int len = TARGET.size();
    // int offspring_fitness = 0;
    // // #pragma omp parallel for shared(TARGET, chromosome) reduction(+:fitness)
    // for(int i=0; i<len; i++) {
    //     if(chromosome[i] != TARGET[i]){
    //         offspring_fitness++;
    //     }
    // }

    // this->fitness = offspring_fitness;
    
// };


typedef struct Individual {
    int fitness;
    char chromosome[targetSize];

} person;




 
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
bool compare(struct Individual ind1, struct Individual ind2) {
    return ind1.fitness < ind2.fitness;
}
 
// Driver code
int main(int argc, char *argv[]) {

    srand(time(0));
    int numtasks, taskid, numworkers, source, mtype, segment, aveseg, extra, offset, rc;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    if (numtasks < 2) {
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }


    char pro_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(pro_name, &name_len);
    numworkers = numtasks - 1;                      // numworkers are workers for process...

    
    // population size...
    struct Individual population[POPULATION_SIZE];
    bool found = false;


    // creating datatype for MPI to pass in send and recv..
    const int nitems = 2;   // elements in struct..
    MPI_Datatype myDataType;
    int blocklengths[2] = {1, targetSize};
    MPI_Datatype types[2] = {MPI_INT, MPI_CHAR};
    MPI_Aint     offsets[2];

    offsets[0] = offsetof(person, fitness);
    offsets[1] = offsetof(person, chromosome);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &myDataType);
    MPI_Type_commit(&myDataType);


    //master task:
    if (taskid == MASTER){

        aveseg = POPULATION_SIZE / numworkers;      // average segmentation for each worker....
        extra = POPULATION_SIZE % numworkers;       // number of workers need to do extra work..
        offset = 0;
        mtype = FROM_MASTER;


        for(int dest=1; dest<=numworkers; dest++) {

            segment = (dest <= extra) ? aveseg + 1 : aveseg;
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&segment, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&population[offset], segment, myDataType, dest, mtype, MPI_COMM_WORLD);
            offset = offset + segment;
        }
        
        mtype = FROM_WORKER;
        for (int i = 1; i <= numworkers; i++) {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&segment, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            // MPI_Recv(&c[offset], segment, MPI_LONG_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&population[offset], segment, myDataType, source, mtype, MPI_COMM_WORLD, &status);
        }


        sort(population, population+POPULATION_SIZE, compare);
        // for(int i=0; i<POPULATION_SIZE; i++){
        //     cout << population[i].chromosome << "   ---" << population[i].fitness << endl;
        // }

    }

    //Worker task:
    if (taskid > MASTER) {
        mtype = FROM_MASTER;
        MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&segment, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&population, segment, myDataType, MASTER, mtype, MPI_COMM_WORLD, &status);

     
        // work...
        // create initial population
        for(int i=0; i<segment; i++) {

            int offspring_fitness = 0;
            int len = TARGET.size();
            int geneLength = GENES.size();
            string gnome = "";
            for(int j=0; j<len; j++){
                gnome += GENES[rand()%geneLength];
                population[i].chromosome[j] = gnome[j];     // putting gnome value in chromosome....
                if(gnome[j] != TARGET[j]){
                    offspring_fitness++;
                }
            }
            population[i].fitness = offspring_fitness;
        }



        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&segment, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        // MPI_Send(&c, segment, MPI_LONG_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&population, segment, myDataType, MASTER, mtype, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
 








    // // create initial population
    // for(int i=0; i<POPULATION_SIZE; i++) {

    //     int offspring_fitness = 0;
    //     int len = TARGET.size();
    //     int geneLength = GENES.size();
    //     string gnome = "";
    //     for(int j=0; j<len; j++){
    //         gnome += GENES[rand()%geneLength];
    //         population[i].chromosome[j] = gnome[j];     // putting gnome value in chromosome....
    //         if(gnome[j] != TARGET[j]){
    //             offspring_fitness++;
    //         }
    //     }
    //     population[i].fitness = offspring_fitness;
    // }

    // for(int i=0; i<POPULATION_SIZE; i++){
    //     cout << population[i].chromosome << "   ---" << population[i].fitness << endl;
    // }

    // sort(population, population+POPULATION_SIZE, compare);






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

    //     for(int i=0; i<s; i++)
    //         new_generation[i] = population[i];


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

    //         population = new_generation;
    //         cout<< "Generation: " << generation << "\t";
    //         cout<< "String: "<< population[0]->chromosome <<"\t";
    //         cout<< "Fitness: "<< population[0]->fitness << "\n";

    //         generation++;
    // }

    // cout<< "Generation: " << generation << "\t";
    // cout<< "String: "<< population[0]->chromosome <<"\t";
    // cout<< "Fitness: "<< population[0]->fitness << "\n";
 

}
