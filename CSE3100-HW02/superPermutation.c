#include <stdio.h>
#include <stdlib.h>

/****************
 * Bryan Arnold *
 *  Homework 2  *
 *   CSE 3100   *
 *   9/27/2017  *
 ***************/

/* readAndCheck function prototype */
void readAndCheck(int len);

/* Main function to run the program */
int main(){
   
	int num;

	/* Ask num input, aka how many numbers to go from 1 to n */
	scanf("%ld", &num);

	if(num < 1) {

		printf("The given number must at least be 1, since permutations go from 1 to n\n");
		return 1;
	
	}

	/* As long as the given value isn't 0 */
	while(num != 0){
	
		/* Check if 1 to num is a superpermutation then ask for another number */
		readAndCheck(num);
		scanf("%ld", &num);

	}

	return 0;

}

/**************************************
 * readAndCheck function implementation.
 * This function takes in a length of
 * 1 to n and checks whether or not the
 * next given numbers create a superpermutation
 * from 1 to n. Returns nothing, just checks
 * if given permutation is a superpermutation.
 *************************************/

void readAndCheck(int len){

	int perm[len];
	int i;

	/* Get input for the permutation */
	for(i = 0; i < len; i++){

		scanf("%ld", &perm[i]);

	}   

	for(i = 0; i < len; i++){

		/* Checker for the positions on whether or not there are superpermutations */
		if((i + 1) != (perm[perm[i]-1])){

			printf("not super\n");
			break;		

		}

	}

	if(i == len){

		printf("super\n");

	}

}

