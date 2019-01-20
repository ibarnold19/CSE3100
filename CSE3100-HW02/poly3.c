#include <stdlib.h>
#include <stdio.h>
#include "reader.h"

/****************
 * Bryan Arnold *
 *  Homework 2  *
 *   CSE 3100   *
 *   9/27/201   *
 ***************/

double poly(double x, int n, double coefs[]){

   double result = 0.0;
   int i;

   for(i = n - 1; i >= 0; i--){

	result = result * x + coefs[i];

  }

  return result;

}

// Write a function called "poly" that takes
// -x the value of the independent variable
// -n the degree of the polynomial
// -coefs the array of coefficients (note that there are n+1 coefficients!)
// And evaluates the polynomial at x. Your implementation must use Horner's rule
// to evaluate P(x). 


int main(int argc,char* argv[]){
   // Write a program that repeatedly (until the degree is -1)
   // Reads a degree
   // reads a polynomial of specified degree
   // reads a value for the independent variable x
   // evaluates the polynomial at that value
   // prints out the value of the polynomial with the string "polynomial evaluate to: %lf\n"
   // returns 0 when it terminates.
   // PS1: You can assume that the degree is either -1 or positive (>= 0)
   // PS2: This should be the same main function as in poly2.c

	/* Infinite loop untilt he user wants to stop */
	while(1){

		int n = 0;
		double x = 0.0;

		scanf("%d", &n);
		
		/* When user wants to stop, enter -1, exit program */
		if(n == -1){

		 	break;
			return 0;

		/* Degree must be >= 0 */
		} else if(n < -1){

			printf("The degree must be >= 0, except -1 for exiting\n");
			return 1;

		}

		/* Allocate appropriately sized array to put desired values into next */
		double coefs[n + 1];

		/* Read reader.c to see how this works */
		readPoly(n, coefs);

		printf("x ? ");
		scanf("%lf", &x);

		/* Display computation */
		printf("polynomial evaluate to: %lf\n", poly(x, n+1, coefs));

	}  

}
