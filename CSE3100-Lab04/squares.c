#include <stdio.h>
#include <stdlib.h>

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *    Lab 04    *
 *   9/30/17    *
 ***************/

/* Computes the sum of the array */
int sum(int n, int* arr) {
	int i, sum;

	for(i = 1; i <= n; i++)
		sum += arr[i];

	return sum;

}

/* Fills the array with the values
 * 1^2, 2^2, 3^2, ..., (n-1)^2, n^2 */
void fillSquares(int n, int* arr) {
	int i;
	for(i = 1; i <= n; i++)
		arr[i] = i*i;

	
}

/* Reads an integer n from arguments,
 * fills array with squares, and computes
 * the sum of the squares. Prints out the
 * sum before freeing all used memory. */
int main(int argc, char* argv[]) {
	
	int n, total;
	int* arr;
	
	if(argc < 2 || argc >= 3) {
		printf("usage: ./squares n\n");
		return 1;
	}
	
	n = atoi(argv[1]);

	if(n < 0) {

	printf("n must be greater than or equal to 0\n");
	return 1;

	}	

	arr = (int*)malloc(sizeof(int)*n);
	
	fillSquares(n, arr);
	total = sum(n, arr);
	printf("total: %d\n", total);
	
	free(arr);
	return 0;

}

