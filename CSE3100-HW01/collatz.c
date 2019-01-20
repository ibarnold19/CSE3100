#include <stdio.h>
#include <stdlib.h>

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *  Homework 1  *
 *    9/17/17   *
 ***************/    

/*protoype deinition of collatz function*/
int collatz(long n, int counter);


/**********************************
 * Collatz function implementation.
 * Uses recursion to reduce a 
 * number to 1 using the collatz
 * procedure, then returns the 
 * number of steps required to get
 * that number to 1.
 * *******************************/

int collatz(long n, int counter){

	if(n == 1) return counter; /* base case */

	/* for even numbers */
	if(n % 2 == 0){
	
		n = n / 2;
		return collatz(n, counter + 1);

	/* for odd numbers */
	} else {

		n = (n * 3) + 1;
		return collatz(n, counter + 1);	


	}	

} /* end function */

int main(int argc,char* argv[]){

	long a, b, valueMax;
	int min, max = 0, steps,  unique = 1, counter = 0;

	printf("bounds:");
	scanf("%ld %ld", &a, &b); /* Get input from user */
	
	if(a < 0 || b < 0){

	printf("The bounds must be greater than or equal to 0\n");
	return 1;

	}

	/* Ierate through the bounds, doing the collatz function on each value */
	for(long i = a; i <= b; i++){

		steps = collatz(i, counter);

		if(steps == max){

			unique = 0; /* highest maximum steps
				     * was matched again, so not unique */

		}
		
		/* New highest number of steps, new unique solution */
		if(steps > max){

			valueMax = i;
			max = steps;
			unique = 1;

		/* If no maximum value is unique, replace with the smaller one */
		} if(unique == 0 && valueMax > i){

			valueMax = i;

		}
	}

		printf("max step for %d is: %d\n", valueMax, max);

	return 0;

}
