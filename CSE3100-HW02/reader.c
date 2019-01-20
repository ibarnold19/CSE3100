#include "reader.h"
#include <stdio.h>

/****************
 * Bryan Arnold *
 *  Homework 2  *
 *   CSE 3100   *
 *   9/27/2017  *
 ***************/

/***********************************
 * readPoly function implementation.
 * This function takes in the degree, 
 * number of coefficients to be put
 * into the polynomial, as well as asks
 * the user the desired number for each
 * coefficient and places then into an
 * array at the order input is given.
 * Returns nothing, just alters the array.
 **************************************/

void readPoly(int degree,double coefs[degree + 1]){
  
	int i;
 
	for(i = 0; i < degree + 1; i++){

		scanf("%lf", &coefs[i]);

	}

}
