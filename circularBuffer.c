#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>

#include "circularBuffer.h"
//#include "generator.h"




/**
 * 
 * @brief decrements freeSemaphore, sets the fbarcsets[writeposition] to the given edge set, increments the write position,increments freeSemaphore
 * @return 1 when no error, -1 when there was an error, 0 supervisor terminating
 */
int writeBuffer(particularSolution* sol){
    
    while(sem_wait(freeSem) != 0){
        if(errno == EINTR){
            continue;
        }
        else
        {
            return -1;
        }
    }
    while(sem_wait(isFreeForWritingSem) != 0){
        if(errno == EINTR){
            continue;
        }
        else
        {
            return -1;
        }
    }
    buff->solutions[buff->writePos] = *sol; //sets the fbarcsets[writeposition] to the given edge set
    printf("Written a solution with %d edges",buff->solutions[buff->writePos].amount);

    if (sem_post(usedSem) != 0) {
		return -1;
	}

    buff->writePos = (buff->writePos + 1 ) % NR_OF_ELEMENTS_IN_BUFFER; // increments the write position
    
    if (sem_post(isFreeForWritingSem) != 0) {
		return -1;
	}

    return 1; 
}



/**
 * @brief increments the read position, saves the current solution in retVal, increments freeSemaphore, decrements usedSemaphore
 * @returns 1 when there was no error, -1 when there was an error, 0 if no generator is running
 */
int readBuffer(particularSolution* retVal){
    if(getRunningStatus() == 0){
        return 0;
    }
    while (sem_wait(usedSem) != 0) {
		if(errno == EINTR)
			continue;
		else
            return -1;
	}
    
    *retVal = buff->solutions[buff->readPos];
    printf("Read from position %d", buff->readPos);

    buff->readPos = (buff->readPos + 1) % NR_OF_ELEMENTS_IN_BUFFER; //increments the read position

    if (sem_post(freeSem) != 0) {
		return -1;
	}
    
    return 1;    
}

/**
 * 
 */
int initialiseBuffer(){
    shmFD = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600);
    if(shmFD == -1){
        fprintf(stderr,"Shared Memory load failed.");
        closeBuffer();
        return -1;
    }
    
    if (ftruncate(shmFD, sizeof(buffer)) < 0){
        fprintf(stderr,"Error in truncate.");
        closeBuffer();
        return -1;
    }
    buff = mmap(NULL, sizeof(*buff), PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);
    if(buff == MAP_FAILED){
        fprintf(stderr,"Map Failed");
        closeBuffer();
        return -1;
    }
    printf("Succsessful to this point.");

    errno = 0;
    
    if((freeSem = sem_open(SEM_FREE, O_CREAT, 0600, NR_OF_ELEMENTS_IN_BUFFER)) == SEM_FAILED){
        fprintf(stderr, "sem_open() (freeSem) failed.  errno:%d\n", errno);
        closeBuffer();
        return -1;
    }
    if((usedSem = sem_open(SEM_USED, O_CREAT, 0600, 0)) == SEM_FAILED){
        fprintf(stderr, "sem_open() (usedSem) failed.  errno:%d\n", errno);
        closeBuffer();
        return -1;
    }
    if((isFreeForWritingSem = sem_open(SEM_ISFREEFORWRITING, O_CREAT, 0600, 1)) == SEM_FAILED){
        fprintf(stderr, "sem_open() (isFreeForWritingSem) failed.  errno:%d\n", errno);
        closeBuffer();
        return -1;
    }
    setRunningStatus(0);
    printf("Generators may be started.");
    return 1;
}

/**
 * 
 */
void closeBuffer(){
    errno = 0;

    if(munmap(buff,sizeof(*buff)) == -1){
        fprintf(stderr,"Error in munmap , errno = %d",errno);
    }
    if(close(shmFD) == -1){
        printf("Error in closing shared memory. errno = %d",errno);
        //ERROR_EXIT();
    }
    if(sem_close(freeSem) == -1){
        printf("Error when closing semaphore.errno = %d",errno);
    }

    if(sem_close(usedSem) == -1){
        printf("Error when closing semaphore.errno = %d",errno);
        //ERROR_EXIT();
    }
    if(sem_close(isFreeForWritingSem) == -1){
        printf("Error when closing semaphore.errno = %d",errno);
    }

    if(sem_unlink(SEM_FREE) == -1){
        printf("Error when unlinking semaphore.errno = %d",errno);
    }
    if(sem_unlink(SEM_USED) == -1){
        printf("Error when unlinking semaphore.errno = %d",errno);
        //ERROR_EXIT();
    }
    if(sem_unlink(SEM_ISFREEFORWRITING) == -1){
        printf("Error when unlinking semaphore.errno = %d",errno);
        //ERROR_EXIT();
    }
    fprintf(stdout,"Buffer closing successful.");

}


/**
 * @brief initialises shmFD, mapped object (buff), semaphores, error handling is in the function
 * 
 */
void setupBufferForOneGenerator(){
	
	shmFD = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0600);
	if(shmFD == -1){
		printf("Error in opening shared mem.");
	}
	buff = mmap(NULL, sizeof( * buff), PROT_READ | PROT_WRITE, MAP_SHARED, shmFD, 0);
	if (buff == MAP_FAILED) {
    	printf("Error in mapping virtual memory.");
	}
	freeSem = sem_open(SEM_FREE, NR_OF_ELEMENTS_IN_BUFFER);
	usedSem = sem_open(SEM_USED, 0);
	isFreeForWritingSem = sem_open(SEM_ISFREEFORWRITING, 1);
	if(freeSem == SEM_FAILED || usedSem == SEM_FAILED || isFreeForWritingSem == SEM_FAILED){
		printf("Error in opening semaphore in generator");	
	}
	
	
	
}
/**
 * @brief closes munmap file, semaphores and shared memory, error handling in function
 * 
 */
void closeBufferOfOneGenerator(){

	if (munmap(buff, sizeof(*buff)) == -1) {
        fprintf(stderr, "munmap ERROR\n");
    }

    if (close(shmFD) == -1) {
        fprintf(stderr, "close shmfd ERROR\n");
    }

    if (sem_close(freeSem) == -1) {
        fprintf(stderr, "free sem close ERROR\n");
    }

    if (sem_close(usedSem) == -1) {
        fprintf(stderr, "used sem close ERROR\n");
    }

    if (sem_close(isFreeForWritingSem) == -1) {
        fprintf(stderr, "mutex sem close ERROR\n");
    }
}

/**
 * 
 * @returns 0 when no error, -1 when there was an error
 */
int setRunningStatus(int status){
    while (sem_wait(isFreeForWritingSem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}

	buff->isRunning = status;

	if (sem_post(isFreeForWritingSem) != 0) {
		return -1;
	}
	return 0;
}

/**
 * 
 * @returns the value of the isRunning variable, -1 when there was an error
 */
int getRunningStatus(){
    int retVal;
    while (sem_wait(isFreeForWritingSem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}

	retVal = buff->isRunning;

	if (sem_post(isFreeForWritingSem) != 0) {
		return -1;
	}
	return retVal;
}

/**
 * @returns 0 when no error, -1 when there was an error
 */
int incRunningStatus(){
    //using isFreeForWritingSem as mutex semaphore so that the modifying operations on the buffer stay atomic
    while (sem_wait(isFreeForWritingSem) != 0) {
		if(errno == EINTR)
			continue;
		else
			return -1;
	}

	buff->isRunning++;

	if (sem_post(isFreeForWritingSem) != 0) {
		return -1;
	}
	return 0;
}

