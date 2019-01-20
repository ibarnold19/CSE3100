#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *    Lab 07    *
 *  11/10/2017  *
 ***************/

//Manage flags for threads
pthread_mutex_t m;

/* This structure stores all of the
 * variables that are shared between
 * the two threads. Handle with care. */
typedef struct {
	int       idx;  /* index of this thread */
	double *a, *b;  /* vectors to dot product */
	int       dim;  /* dimensionality of vectors */
	double   psum;  /* partial dot product */
} thread_data;

/* This is the function each thread will call
 * to do the work of computing the partial dot
 * product. */
void* worker(void* arg) {
	thread_data* dat;
	int     idx, dim;
	double    *a, *b;
	
	/* Get thread arguments */
	dat = (thread_data*) arg;
	idx = dat->idx;
	a = dat->a;
	b = dat->b;
	dim = dat->dim;
	dat->psum = 0;	//Set partial sums to 0

	int i; //incrementing
	double psum = 0; //psum of one thread goes into this, will later be the struct's sum

	for(i = 0; i < dim; i++){

		pthread_mutex_lock(&m); //Wait until data is available to change
		psum = psum + (a[i] * b[i]); //Do dot product of vector element
		pthread_mutex_unlock(&m); //Thread done using data, next thread can use.

	}

	dat->psum = psum; //Set partial sum to the struct's sum
	return dat; //return the struct pointer

	/* TODO compute partial dot product 
	 * and safely exit the thread... */
}

int main(int argc, char* argv[]) {
	
	pthread_t tid1, tid2; //two thread ids
	int dim = 0, status = 0; //dimension of the vectors, and the status of their creation
	pthread_mutex_init(&m, NULL); //initialize thread flags
	
	/* Parse program arguments */
	if(argc != 2) {
		printf("usage: ./dot <dimensionality>\n");
		exit(2);
	}

	dim = atoi(argv[1]);

	int i = 0, j = 0; //Incrementors

	//Create space for the vectors a and b
	double *a = (double*)malloc(sizeof(double)*dim);
	double *b = (double*)malloc(sizeof(double)*dim);
	
	thread_data dat; //Create new struct for thread data
	dat.idx = 0; //initialize
	dat.dim = dim; //dim to dim
	dat.psum = 0; //psum is 0 to start

	//This creates the elements for a vector, from 1 to n
	for(i = 0; i < dim; i++){

		a[i] = i+1;

	}

	//This creates the elements for b vector, from n to 1
	int k = 0;
	for(j = dim; j > 0; j--){

		b[k] = j;
		k++;

	}

	//Set vectors to struct's vectors
	dat.a = a;
	dat.b = b;

	//Pointer tot he struct
	thread_data* pt =  &dat;

	//Create threads and have them both run worker function
	status = pthread_create(&tid1, NULL, (void*(*)(void*))worker, (void*)pt);
	status = pthread_create(&tid2, NULL, (void*(*)(void*))worker, (void*)pt);

	//join the threads back into the main thread with the psum result for the vector dot product and 
	//destroy the other two created threads
	void *v1, *v2;
	pthread_join(tid1, &v1);
	pthread_join(tid2, &v2);
	pthread_mutex_destroy(&m);

	//Print out psum and free a and b
	double answer = dat.psum;
	printf("ans = %lf\n", answer);
	free(a);
	free(b);

	/* TODO initialize thread data structure,
	 * create threads, wait for them to terminate,
	 * add the partial sums together, then print! */
	
	/* Be sure not to have any memory errors/leaks!! */
	
	/* Done! */
	return 0;
}
