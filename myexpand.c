#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "myexpand.h"


/**
 * @brief function for printing the usage message, upon incorrect use of the program
 * @param string with the name of the file
 * @return prints the usage message and exits the program with EXIT_FAILURE
*/
void myUsage(char *fileName){ 
	fprintf(stderr, "%s\t[USAGE]\t %s [-t tabstop] [-o outputFile] [file...]\n", fileName, fileName); 
	exit(EXIT_FAILURE); 
} 

/**
 * variable for the number of spaces to replace the tabulator with
 */
long tabstop = 8;


/**
 * @brief replaces the tabulator in the inFile with a certain number of spaces and writes the result in outFile
 * @param input file and the output file
 * @returns -1 in case of error, 1 if there was no error
 *
*/
int readFile(FILE *inFile, FILE *outFile){
	int currChar;
	int writePosition = 0;
	
	while( (currChar = fgetc(inFile)) != EOF){

		if(feof(inFile)){
			break;
		}
		// printf("%c");
		if(currChar == '\t'){
			int p  = tabstop * ((writePosition/tabstop) + 1); // formula given in the assignment sheet
			while (writePosition < p)
			{
				if(fputc(' ', outFile) ==EOF){
					printf("fputs failed\n");
					exit(EXIT_FAILURE);
				}
				writePosition++;
			}
		}
		else
		{	
			if(fputc(currChar, outFile) == EOF){
				printf("fput failed\n");
				exit(EXIT_FAILURE);
			}

			if(currChar == '\n'){
				writePosition = 0;
			}
			else{
				writePosition++;
			}
		}
		if(ferror(inFile)){
			printf("there was an error in input file\n");
			if(fclose(inFile) != 0){
				printf("there was an error in closing the problematic input file\n");
			}
			exit(EXIT_FAILURE);
		}
	}
	if(inFile == stdin){
		fflush(stdin);
	}
	else
	{
		if(fclose(inFile) != 0){
			printf("Readfile wasn't closed");
			exit(EXIT_FAILURE); 
		}
	}
	return 1;
}


/**
 * @brief the main function of the program 
 * @param arguments of the console
 * @return 1 if executed without errors
 * 
 * 
*/
int main (int argc, char *argv[]){
	char *fileName = argv[0]; // setting the file name (myexpand)
	int c; // for getopt results
	FILE *out = stdout;
	char **ptr = malloc(sizeof(char) * 10); // pointer for the function strtol
	
	while ( (c = getopt(argc,argv,"t:o:")) != -1) // t and o both require arguments
	{
		switch(c){
			case 't': 
				tabstop = strtol(optarg, ptr, 10); 
				break;
			case 'o': if((out = fopen(optarg, "w+")) == NULL){
					printf("fopen in getopt failed, please use one of the valid file formats for the output file \n");
					myUsage(fileName);
				} 
				break;
			case '?': myUsage(fileName); 
				break; 
		}
	}
	if(tabstop <= 0){
		printf("Value of -t is invalid\n"); 
		exit(EXIT_FAILURE);
	}
	
	int numOfPositionalArgs = argc - optind; // = equal to the amount of input files

	if(numOfPositionalArgs>0){ // branch with input files
		FILE *inFiles[numOfPositionalArgs];
		for(int i = 0; i<numOfPositionalArgs; i++){
			
			if((inFiles[i] = fopen(argv[optind+i], "r")) == NULL){
				printf("Fopen Failed, invalid name of the input file\n");
				exit(EXIT_FAILURE);
			}
			
			if( readFile(inFiles[i], out) == -1){
				printf("Error in readFILE \n");
				exit(EXIT_FAILURE);
			}
		}
	}
	else{ //branch without input files
		printf("There were no input files found. Please input the text to be expanded in the console and press Enter.\n (To exit use 'Ctrl + D')\n");
		if(readFile(stdin, out) == -1){
			printf("ErrorInRead line 82");
			exit(EXIT_FAILURE);
		}
	}
	fclose(out);
	exit(EXIT_SUCCESS);
	return 1;
}
