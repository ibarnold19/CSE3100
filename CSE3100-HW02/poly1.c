#include <stdio.h>
#include <stdlib.h>

/****************
 * Bryan Arnold *
 *  Homework 2  *
 *   CSE 3100   *
 *   9/27/2017  *
 ***************/

/******************************
 * power function implmentation
 * This function takes a double,
 * the number to be risen to a 
 * power, and the degree in which
 * to compute the exponent of the
 * given double, and returns the
 * resultant exponent value.
 *******************************/

double power(double x,int n){

  /*Base case, to the power of 1 is just the number */
  if(n == 1) return x;

  /* If even */
  if(n % 2 == 0){
	
	return power(x, n/2) * power(x, n/2);

  /* If odd */
  } else {

	return x * power(x, n/2) * power (x, n/2);

  }


}

/* Main function */
int main(int argc,char* argv[]){

  /* Correct number of arguments */
  if (argc < 3) {
   
	printf("Usage: power base exp\n");
    	return 1;

  }

  double base = strtod(argv[1],NULL);
  int    exp  = atoi(argv[2]);
  
  /* Exponent to be risen must be positive */
  if(exp < 0) {

	printf("Exponent must be greater than or equal to 0.\n");
	return 1;

  }

  /* Default to 1 if exponent is 0 this covers it */
  double ans = 1.0;
 
  /* REsult not to the 0 power, do computation */
  if(exp > 0){

 	ans = power(base, exp);

  }

  printf("answer = %lf\n",ans);

  return 0;

}
