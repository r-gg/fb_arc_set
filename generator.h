#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>



#ifndef generator_H_
#define generator_H_
#include "circularBuffer.h"
#define MAX_NR_OF_GENERATES_BEFORE_TERMINATION 10

int generatorTerminateFlag; //if set to 1 then the generator terminates

void incGeneratorTerminateFlag();

/**
 * @brief function for printing the usage message, upon incorrect use of the program
 * @param string with the name of the file
 * @return prints the usage message and exits the program with EXIT_FAILURE
*/
void myUsage(char *fileName);





void setupBufferForOneGenerator();

void closeBufferOfOneGenerator();

int calculateOneSolution(edge* edges, int numberOfEdges, edge* fbArcSet, int* permutation, int numberOfVertices);

void extractOnlyNeededEdges(edge* fbArcSet, int numOfNeededEdges, edge* resultFbSet);


/**
 * @brief Fisher Yates shuffled integers are saved in the array permutation
 * @pre size of the array permutation must be equal to numberOfVertices
 * @return 
 */
void getPermutation(int* permutation, int numberOfVertices);


/**
 * @param array and the searched element
 * @returns returns the index of the searched element
 */
int findIndexOfElem(int *permutations, int element);

/**
 * 
 * @brief stores the edges (from argv[][] in the array parameter edges
 * @param array where the edges will be stored (must have the length = numberOfEdges), console arguments
 * @returns the number of vertices 
 */
int setEdgesAndGetNumberOfVertices(edge *edges,int numberOfEdges, char *argv[]);

/**
 * @brief the main function of the program 
 * @param arguments of the console
 * @return 1 if executed without errors
*/
int main (int argc, char *argv[]);


#endif
