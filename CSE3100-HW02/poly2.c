#include <stdio.h>
#include <stdlib.h>
#include "reader.h"

/****************
 * Bryan Arnold *
 *  Homework 2  *
 *   CSE 3100   *
 *  9/27/2017   *
 ***************/

/* Powers prototype */
double powers(double x1, int n1);

//poly:

//Write a function called "poly" that takes
//-x the value of the independent variable
//-n the degree of the polynomial
//-coefs the array of coefficients (note that there are n+1 coefficients!)
//And evaluates the polynomial at x. Feel free to make use of your power 
//function.
//
double poly(double x, int n, double coefs[]){

	/* a[0] added to start */
	double result = coefs[0];
	int i;

	/* Add on to result using appropriate power */
	for(i = 1; i < n + 1; i++){

		result = result + coefs[i] * powers(x, i);

	}

	return result;

}

/* Duplicate method from polyc.1, named differently.
 * Wasn't allowed to access my previously created 
 * function in poly1.c for some reason, further
 * problems with this addressed in my README.TXT.
 * Making a poly1.h file didn't fix the problem.
 * REad poly1.c power function for how this function
 * works as it is an exact copy */

double powers(double x1, int n1){
	
	/* Base case, to the power of 1 is just the number */
	if(n1 == 1) return x1;

	/* If even */
	if(n1 % 2 == 0){

		return powers(x1, n1/2) * powers(x1, n1/2);
	
	/*If odd */
	} else {

		return x1 * powers(x1, n1/2) * powers(x1, n1/2);

	}

}
 //main:
 
 //Write a program that repeatedly (unitl the degree is -1)
 //Reads a degree
 //reads a polynomial of specified degree
 //reads a values for the independent variable x
 //evaluates the polynomial at that value
 //prints out the value of the polynomial with the string "polynomial evaluate to: %lf\n"
 //returns 0 when it terminates
 //PS1; You can assume the degree is either -1 or positive (>=0)
 //PS2: ALL the variables should be AUTOMATIC (on the stack). DO NOT USE MALLOC/FREE
int main(){
	
	/* Infinite loop until user wants to end program */
	while(1){

		int n = 0;
		double x = 0.0;

		scanf("%d", &n);
		
		/* Exits the program */
		if(n == -1){

		 break;
		 return 0;		
	
		/* Degree must be >= 0, or -1 */
		} else if(n < -1){

			printf("The degree must be >= 0, except -1 for exiting\n");
			return 1;			

		}

		/* Create appropraitely sized array for allocation of values next */
		double coefs[n + 1];
		
		/* See reader.c for how this works */
		readPoly(n, coefs);
		printf("x ? ");
		scanf("%lf", &x);

		/* Display computation */
		printf("polynomial evaluate to: %lf\n", poly(x, n, coefs));

	}

}
