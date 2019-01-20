#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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
	
	int i, start, end;
	double psum;
	
	/* Get thread arguments */
	dat = (thread_data*) arg;
	idx = dat->idx;
	a = dat->a;
	b = dat->b;
	dim = dat->dim;
	
	/* Determine elements to be summed */
	start = idx * dim / 2;
	end = start + dim / 2;
	if(dim % 2 == 1 && idx == 1)
		end += 1;
	
	/* Compute partial sum */
	psum = 0;
	for(i = start; i < end; i++)
		psum += a[i] * b[i];
	
	/* Store partial sum in arg */
	dat->psum = psum;
	
	/* Exit thread cleanly */
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	
	int dim;
	
	thread_data dat1, dat2;
	pthread_t t1, t2;
	int status, i;
	double ans;
	
	/* Parse program arguments */
	if(argc != 2) {
		printf("usage: ./dot <dimensionality>\n");
		exit(2);
	}
	dim = atoi(argv[1]);
	
	/* Initialize thread data structures */
	dat1.a = dat2.a = (double*) malloc(dim * sizeof(double));
	dat1.b = dat2.b = (double*) malloc(dim * sizeof(double));
	if(dat1.a == NULL || dat1.b == NULL) {
		printf("error: failed to allocate vectors!\n");
		exit(1);
	}
	dat1.idx = 0;
	dat2.idx = 1;
	dat1.dim = dat2.dim = dim;
	
	/* Initialize vectors */
	for(i = 0; i < dim; i++) {
		dat1.a[i] = i+1;
		dat1.b[i] = dim-i;
	}
	
	/* Create threads */
	status = pthread_create(&t1, NULL, worker, (void*)&dat1);
	if(status != 0) {
		printf("error: failed to create thread 1!\n");
		exit(2);
	}
	status = pthread_create(&t2, NULL, worker, (void*)&dat2);
	if(status != 0) {
		printf("error: failed to create thread 2!\n");
		exit(3);
	}
	
	/* Wait for threads to terminate */
	status = pthread_join(t1, NULL);
	if(status != 0) {
		printf("error: failed to join thread 1!\n");
		exit(4);
	}
	status = pthread_join(t2, NULL);
	if(status != 0) {
		printf("error: failed to join thread 2!\n");
		exit(5);
	}
	
	/* Compute final dot product from partials */
	ans = dat1.psum + dat2.psum;
	
	/* Output the result */
	printf("ans = %lf\n", ans);
	
	/* Clean up! */
	free(dat1.a);
	free(dat1.b);
	
	/* Done! */
	return 0;
}
