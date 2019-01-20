#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *  Homework 1  *
 *    9/17/17   *
 ***************/

/* Prototype defintion for fibSumEven function */
long long fibSumEven(long lim);

/* Prototype definition for fibSumPrime function */
long long fibSumPrime(long lim);

/************************************
 * fibSumEven function implementation.
 * This function finds the summation
 * of all the even Fibonacci numbers
 * below a given limit.
 ***********************************/

long long fibSumEven(long lim){

	long long n1 = 0, n2 = 2, n3;
	long long sum = n1 + n2;

	/* No even numbers below 2, so just return 0 */
	if(lim <= 2) return 0;

	/* Loop to look at each Fibonacci number */
	while(n2 < lim){

		/* This computation is a way to get each even Fibonacci number*/
		n3 = 4 * n2 + n1;
		
		/* When the next even number is over the limit, end */
		if(n3 >= lim) break;

		n1 = n2;
		n2 = n3;
		sum += n2;

	}

	return sum;

} /* End function */

/**************************************
 * fibSumPrime function implementation.
 * This function finds the summation
 * of all the prime Fibonacci numbers
 * less than a given limit and returns
 * it.
 ************************************/

long long fibSumPrime(long lim){

	long long n1 = 2, n2 = 3, n3;
	long long sum = n1 + n2;
	int isPrime;

	/* No prime numbers lower than this, so return 0 */
	if(lim <= 2) return 0;

	/* Use this case just to make computations easier */
	if(lim == 3) return 2;

	/* Loop to check all Fibonacci numbers */
	while(n2 < lim){

		n3 = n1 + n2;

		/* Assume all numbers prime until proven otherwise */
		isPrime = 1;

		/* This loop checks if the computed next Fibonacci number
 		 * n3 is prime or not. It checks if half the value is divisible
 		 * by any other number. If it is, it isn't prime. */
		for(int i = 2; i <= n3/2; ++i){

			if(n3 % i == 0){
				
				/* Indicator that given number isn't prime */
				isPrime = 0;
				break;

			}	

		}	
		
		/* Make sure current number isn't over limit */
		if(n3 >= lim) break;
		
		/* Add to sum if prime, otherwise just iterate to next Fibonacci		 *number */
		if(isPrime == 1){

			n1 = n2;
			n2 = n3;
			sum += n3;
			
		} else {

			n1 = n2;
			n2 = n3;

		}
	
	}

	return sum;

} /* End function */

int main(int argc,char* argv[]){

	long long sumEven, sumPrime;
	long lim;

	/* Send message if command line input is incorrect to user */
	if(argc < 2){

		printf("usage is: ./fibhw0 lim\n");
		return 1;

	}
	
	/* Get the limit input from the command line */
	lim = atoi(argv[1]);
	
	if(lim < 0){

		printf("limit must be greater than or equal to 0\n");
		return 1;

	}

	sumEven = fibSumEven(lim);
	sumPrime = fibSumPrime(lim);

	printf("even total = %lld\n", sumEven);	
	printf("prime total =  %lld\n", sumPrime);	

	return 0;

}
