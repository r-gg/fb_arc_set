#include <stdio.h>

#ifndef myexpand_H_
#define myexpand_H_


/**
 * @brief function for printing the usage message, upon incorrect use of the program
 * @param string with the name of the file
 * @return prints the usage message and exits the program with EXIT_FAILURE
*/
void myUsage(char *fileName);

/**
 * variable for the number of spaces to replace the tabulator with
 */
long tabstop;

/**
 * @brief replaces the tabulator in the inFile with a certain number of spaces and writes the result in outFile
 * @param input file and the output file
 * @returns -1 in case of error, 1 if there was no error
 *
*/
int readFile(FILE *inFile, FILE *outFile);

/**
 * @brief the main function of the program 
 * @param arguments of the console
 * @return 1 if executed without errors
*/
int main (int argc, char *argv[]);


#endif
