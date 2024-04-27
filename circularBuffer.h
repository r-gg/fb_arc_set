#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>

//#include "generator.h"


#ifndef circBuff_H_
#define circBuff_H_

#define NR_OF_ELEMENTS_IN_BUFFER 5 
#define MAX_RET_EDGES 8
#define ERROR_EXIT() {closeBuffer(); exit(EXIT_FAILURE);}

#define SEM_FREE "/freeSemaphore6228"
#define SEM_USED "/usedSemaphore132sss51"
#define SEM_ISFREEFORWRITING "/isFreeForWriting1642s781"
#define SHM_NAME "/shmBuffer423"

typedef int vertex;

typedef struct edge
{
	vertex startVertex;
	vertex endVertex;
} edge;


sem_t *freeSem;
sem_t *usedSem;
sem_t *isFreeForWritingSem; /** @brief 1 if the buffer is currently being written, 0 if the buffer is free for writing // Mutex */



int shmFD; //file descriptor for shared memory file

typedef struct particularSolution{
    edge fbArcSet[MAX_RET_EDGES]; //edges of this particular solution
    int amount; // number of edges in this particular solution
} particularSolution;

typedef struct buffer
{
    int isRunning; // 0 when false, 1 if true // supervisor sets it accordingly
    int readPos;
    int writePos;
    particularSolution solutions[MAX_RET_EDGES]; // array with max 8 elements, where the elements are the arrays in one FB arc set (generators write them)
}buffer;

buffer *buff;



/**
 * 
 * @brief decrements freeSemaphore, sets the fbarcsets[writeposition] to the given edge set, increments the write position,increments freeSemaphore
 * @return 1 when no error, -1 when there was an error, 0 supervisor terminatings
 */
int writeBuffer(particularSolution* sol);



/**
 * @brief increments the read position, saves the current solution in retVal, increments freeSemaphore, decrements usedSemaphore
 * @returns 1 when there was no error, -1 when there was an error
 */
int readBuffer(particularSolution * retVal);

/**
 * @brief sets up semaphores, shared memory, virtual memory. Upon error closes the buffer. [ OF THE MAIN BUFFER ]
 * 
 * @return 1 when no error, -1 when there was an error
 */
int initialiseBuffer();

/**
 * @brief closes semaphores, shared memory, virtual memory [ OF THE MAIN BUFFER ]
 */
void closeBuffer();


/**
 * Opens shared memory and semaphores for one generator locally
 */
void setupBufferForOneGenerator();


/**
 * 
 * closes the shared memory and semaphores for one generator locally (without unlink)
 */
void closeBufferOfOneGenerator();

/**
 * 
 * @returns 0 when no error, -1 when there was an error
 */
int setRunningStatus(int status);

/**
 * 
 * @returns the value of the isRunning variable, -1 when there was an error
 */
int getRunningStatus();

/**
 * @returns 0 when no error, -1 when there was an error
 */
int incRunningStatus();


#endif