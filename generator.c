#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>


#include "circularBuffer.h"
//#include "generator.h"







/**
 * @brief function for printing the usage message, upon incorrect use of the program
 * @param string with the name of the file
 * @return prints the usage message and exits the program with EXIT_FAILURE
*/
void myUsage(char *fileName){ 
	fprintf(stderr, "%s\t[USAGE]\t %s EDGE1 ...\n Where the syntax of the EDGE is following: startVertex-endVertex\n start and end vertex are described with non negative integer values\n names of vertices are from 0 to (n-1), where n is the total number of vertices in the graph", fileName, fileName); 
	printf("EXAMPLE: \n \t %s 0-1 1-2 1-3 1-4 2-4 3-6 4-3 4-5 6-0 ", fileName);
	exit(EXIT_FAILURE); 
} 

/**
 * @brief Fisher Yates shuffled integers are saved in the array permutation
 * @pre size of the array permutation must be equal to numberOfVertices
 * @return 
 */
void getPermutation(int* permutation, int numberOfVertices){ //Fisher-Yates shuffle

	for(int i = 0; i < numberOfVertices; i++){ //filling the permutation array with [0,1,2,3,4 ... ,numberofvertices-1]
		permutation[i] = i;
	}
	int j; //random number (index for exchange with the i-th element) ( 0 <= j <= i )
	int tmp;
	
	for(int i = numberOfVertices - 1; i >= 0; i--){
		
		if(i == 0){
			j = rand() % numberOfVertices;
			
			
			tmp = permutation[j];
			permutation[j] = permutation[i];
			permutation[i] = tmp;
			break;
		}
		else{
			
			j = rand() % numberOfVertices;
			tmp = permutation[j];
			
			permutation[j] = permutation[i];
			permutation[i] = tmp; 
			
		}
	}
	
}


/**
 * @param array and the searched element
 * @returns returns the index of the searched element, -1 when there is no element
 */
int findIndexOfElem(int *permutations, int element){
	for(int i = 0; i < sizeof(permutations); i++){
		if(permutations[i] == element){
			return i;
		}
	}
	return -1;
}

/**
 * 
 * @brief stores the edges (from argv[][] in the array parameter edges
 * @param array where the edges will be stored (must have the length = numberOfEdges), console arguments
 * @returns the number of vertices 
 */
int setEdgesAndGetNumberOfVertices(edge *edges,int numberOfEdges, char *argv[]){

	char * fileName = argv[0];
	

	int nameOfTheLastVertex = 0; // maximum vertex name

	for(int i = 0; i < numberOfEdges; i++){
		if(sscanf(argv[1+i], "%d-%d", &edges[i].startVertex, &edges[i].endVertex) != 2){ 
			printf("Error in the given edge list. \n"); // sscanf didn't get to read exactly 2 values
			myUsage(fileName);
		}
		if(edges[i].startVertex < 0 || edges[i].endVertex < 0){
			printf("Only positive integers can be names of vertices");
			myUsage(fileName);
		}

		if(edges[i].startVertex > nameOfTheLastVertex){
			nameOfTheLastVertex = edges[i].startVertex;
		}
		if(edges[i].endVertex > nameOfTheLastVertex){
			nameOfTheLastVertex = edges[i].endVertex;
		}
	}

	int numberOfVertices = nameOfTheLastVertex + 1;

	return numberOfVertices;

}




/**
 * 
 * @brief calculates a solution following the instructions given in the assignment
 * @returns the cnter or the number of edges in fbArcSet that are a solution (elements up to cnter are one fbArcSet)
 */
int calculateOneSolution(edge* edges, int numberOfEdges, edge* fbArcSet, int permutation[], int numberOfVertices){

	getPermutation(permutation,numberOfVertices);
	/*
	printf("\none Permutation of vertices: ");
	for(int i = 0; i < numberOfVertices; i++){
		printf("%d\n", permutation[i]);
	}
	*/
	
	int cnter = 0; // counter of how many elements are there in fbArcSet (numberOfElements-1) (index of the last element)
	
	vertex startV;
	vertex endV;
	for(int i = 0 ; i < numberOfEdges ; i++){
		startV = edges[i].startVertex;
		endV = edges[i].endVertex;

		if(findIndexOfElem(permutation,startV)  < findIndexOfElem(permutation,endV)){
			
			fbArcSet[cnter].startVertex = startV;
			fbArcSet[cnter].endVertex = endV;
			cnter++;
		}
		
	}

	return cnter;

}


/**
 * @param resultFbSet must have the length numOfNeededEdges and numOfNeededEdges <= sizeof(fbArcSet)
 * @brief copies the first n elements from fbarcset to resultfbset (n = numOfNeededEdges)
 */
void extractOnlyNeededEdges(edge* fbArcSet, int numOfNeededEdges, particularSolution partSol){
	for(int i = 0;i<numOfNeededEdges;i++){
		partSol.fbArcSet[i] = fbArcSet[i];
	}
}

int main(int argc, char *argv[]){

	setupBufferForOneGenerator();
	
	char *fileName = argv[0];

	if(argc <= 1){ //check if there are >0 arguments (edges)
		printf("Not enough arguments, enter at least one edge.");
		myUsage(fileName);
	}

	

	int numberOfEdges = argc - 1; //number of positional arguments
	
	edge edges[numberOfEdges];
	
	edge fbArcSet[numberOfEdges];

	int numberOfVertices = setEdgesAndGetNumberOfVertices(edges,numberOfEdges,argv);

	int permutation[numberOfVertices];

	if(incRunningStatus() == -1){
		fprintf(stderr,"couldnt inc running status");
		closeBuffer();
		exit(EXIT_FAILURE);
	}

	srand(time(NULL)); // resetting the start (seed) of the time for the random function (when not done, always returns the same results)

	int runningStatus;
	while((runningStatus = getRunningStatus()) > 0){ // OR with while(buff->isRunning = 1){...}
		
		int cnter = calculateOneSolution(edges,numberOfEdges,fbArcSet,permutation,numberOfVertices); //amount of edges in the particular solution

		


		if(cnter > MAX_RET_EDGES){
			printf("Too many edges in FBAS (%d)", cnter);
		}
		else{

			//edge resultFbArcSet[cnter]; //each result newly declared
			particularSolution partSol = { .amount = cnter};
			extractOnlyNeededEdges(fbArcSet,cnter,partSol); 
			printf("Attempting to write a solution with %d edges.", cnter);
			//particularSolution* pointerToPartSol = &partSol;

			int wrBuff_retVal = writeBuffer(&partSol);
			if(wrBuff_retVal == -1){
				printf("error in writing buffer, closing generator");

				break;
			}
			if(wrBuff_retVal == 0){
				printf("error in writing buffer retVal = 0, closing generator");
				break;
			}

		}
	}
	closeBufferOfOneGenerator();
	return 1;
}