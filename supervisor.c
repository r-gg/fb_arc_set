#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>


#include "circularBuffer.h"

int quit = 0;

#define MAX_RET_EDGES 8
#define MAX_NUM_OF_EDGES_IN_GRAPH 100

#define shmNumOfEdges "/num_of_edges"
#define shmGraph "/graph"

int numOfEdges;

struct graphEdges
{
	edge edges[MAX_NUM_OF_EDGES_IN_GRAPH];
};

void printNewFoundBest(particularSolution* newBestFound){
	printf("Solution with %d edges: ", newBestFound->amount);
}

void mySignalHandler(int sigNum){
	quit++;

}
/**
 * @brief function for printing the usage message, upon incorrect use of the program
 * @param string with the name of the file
 * @return prints the usage message and exits the program with EXIT_FAILURE
*/
void myUsageSupervisor(char *fileName){ 
	fprintf(stderr, "%s\t[USAGE]\t %s \n", fileName, fileName); 
	exit(EXIT_FAILURE); 
} 


int main (int argc, char *argv[]){
	fprintf(stdout,"This is the first line.");
	char *fileName = argv[0];

	struct sigaction sa = { .sa_handler = mySignalHandler };
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	
	if(argc > 1){
		printf("no arguments required");
		myUsageSupervisor(fileName);
	}

	fprintf(stdout,"I got to here.");
	if(initialiseBuffer() == -1){ 
		printf("Buffer initialisation failed.");
		exit(EXIT_FAILURE);
	}

	particularSolution currS;
	particularSolution bestS;
	particularSolution* currentSol = &currS;
	//particularSolution* bestSol = &bestS;
	//currentSol->amount = __INT_MAX__;
	bestS.amount = MAX_RET_EDGES + 1; // 9
	buff->readPos = 0;
	buff->writePos = 0;
	fprintf(stdout,"Generators may be started.");

	//time_t start = time(0);
	int countOfReads = 5; ////FOR TESTING ONLY REMOVE AFTER
	int readResult;
	while(quit == 0){
		
		/*
		if(difftime(time(0),start) >= 10){
			break;
		}*/
		
		if((readResult = readBuffer(currentSol)) == -1){
			printf("error in reading buffer from supervisor");
		}
		else if(readResult == 0)
		{
			continue;
		}
		
		printf("\nBestSol.Amount = %d \n", currentSol->amount);
		if(currentSol->amount < bestS.amount){
			bestS.amount = currentSol->amount;
			printNewFoundBest(currentSol);
		}
		if(bestS.amount == 0){
			printf("Graph is acyclic.");
			break;
		}
		/*
		if(countOfReads <= 0){ //////FOR TESTING ONLY 
			break;
		}
		countOfReads--;*/

	}
	setRunningStatus(0);
	sem_post(freeSem);
	closeBuffer();
	exit(EXIT_SUCCESS);
}
