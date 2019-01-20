#include <stdio.h>
#include <stdlib.h>

/****************
 * Bryan Arnold *
 *  Homework 3  *
 *   CSE 3100   *
 *   10/4/17    *
 ***************/

struct Matrix {
  size_t row;
  size_t col;
  double** mat;
};

typedef struct Matrix TMatrix;

/*
 * Creates and returns a matrix of size rows x cols
 * - rows : (non-negative value) giving the number of rows
 * - cols : (non-negative value) giving the number of columns
 * If the allocation is not successful, the function should return 
 * a matrix of size 0 x 0
 * If the allocation is successful, the mat field of the ADT should
 * point to an array of pointers (representing the rows) and each pointer
 * in that array should point to an array of double representing the values
 * in that row. 
 */
TMatrix makeMatrix(size_t rows,size_t cols ) {
   
	TMatrix new;

	int i;

	new.row = rows;
	new.col = cols;

	/* Pointer to pointer allocation */
	new.mat = (double**)malloc(sizeof(double*)*rows);

	if(new.mat == NULL){

		new.row = 0;
		new.col = 0;
		return new;

	}

	for(i = 0; i < rows; i++){

		/* A pointer to an array is created */
		new.mat[i] = (double*)malloc(sizeof(double)*cols);

		if(new.mat[i] == NULL){

			new.row = 0;
			new.col = 0;
			return new;

		}

	}


	return new;

}

/*
 * This function is responsible for deallocating the dynamic memory
 * currently used by this matrix. Namely, it should deallocate the rows
 * and the array of row storage. 
 */
void freeMatrix(TMatrix m) {
   
	int i;

	for(i = 0; i < m.row; i++){

		/* Need to free each smaller array first */
		free(m.mat[i]);

	}

	/* Free array of pointers last */
	free(m.mat);

}

/*
 * This function takes as input a matrix ADT and reads, from the standard
 * input, a collection of row x col doubles representing the content of the
 * matrix. Note that the content of the matrix should be given in row-major
 * order. Namely, the matrix
 * 1 2 3 
 * 4 5 6 
 * 7 8 9
 * should be conveyed with a white-space separated sequence of 9 numbers:
 * 1 2 3 4 5 6 7 8 9
 * If the input is malformed, the reading should be interrupted and the
 * function shall return the value 0 to report a failed read. 
 * If the input is well-formed, the reading should complete and the function
 * shall return the value 1. 
 */
int readMatrix(TMatrix m) {
  
	int i, j;


	for(i = 0; i < m.row; i++){

		for(j = 0; j < m.col; j++){
		
			/* IF scanf doesn't sucessfully read input */
			if(scanf("%lg", &m.mat[i][j]) != 1){

				return 0;

			}

		}

	}

	return 1;

}

/*
 * The printMatrix function takes a matrix ADT as input and produces, on the
 * standard output, a representation of the matrix in row-major format. For
 * instance, the 3x3 identity matrix should print on 3 lines as:
 * 1 0 0 
 * 0 1 0
 * 0 0 1
 */
void printMatrix(TMatrix m) {
   
	int i, j;

	for(i = 0; i < m.row; i++){

		for(j = 0; j < m.col; j++){

			printf("%lg ", m.mat[i][j]);

		}

	printf("\n");

	}

}

/*
 * The transposeMatrix function takes as input a matrix ADT m and returns a
 * new TMatrix ADT that holds the transpose of m. Transposition should run in 
 * O(n x m)  (where n is the # of rows and m the # of columns). 
 * If memory allocation for the transpose failed, the routine should return
 * a 0x0 matrix ADT. 
 * Transposition follows the usual mathematical definition of transposition. 
 */
TMatrix transposeMatrix(TMatrix m) {

	/* For formatting reasons */
	printf("\n");
	TMatrix new = makeMatrix(m.col, m.row);
   
	int i,j;

	for(i = 0; i < new.row; i++){

		for(j = 0; j < new.col; j++){
			
			/* Rows become cols, and values transition accordingly */
			new.mat[i][j] = m.mat[j][i];

		}

	}

	return new;

}


/*
 * The test program is provided!
 */
int main() {
   int     m, n;

   printf( "Enter m and n (<= 0 to exit)" );
   scanf( "%d %d", &m, &n );

   while( (m > 0) && (n > 0) ) {
       TMatrix a = makeMatrix( m, n);
       if( a.mat ) {
          printf( "Enter %dx%d matrix\n", m, n );
          if (readMatrix(a)) {
            printf( "Input matrix\n" );
            printMatrix(a);
            TMatrix t = transposeMatrix(a);
            if (t.mat) {
              printf( "Transposed matrix\n" );
              printMatrix(t);
              freeMatrix(t);
	      printf("\n"); //Added for formatting purposes
            }
          } else {
            printf("Error while reading the matrix\n");
	    return 1; //Added due to infinite loop error
          }
          freeMatrix(a);
       }
       printf( "Enter m and n (<= 0 to exit)" );
       scanf( "%d %d", &m, &n );
   }
   return 0;
}
