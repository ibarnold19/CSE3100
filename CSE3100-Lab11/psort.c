#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_once_t onceBlock = PTHREAD_ONCE_INIT;
pthread_mutex_t m;
int idWant;

void checkError(int status){

	if(status){

		abort();

	}

}

void initMutexOnce(void){

	int status = pthread_mutex_init(&m, NULL);
	if(status){

		exit(-1);

	}

}

/* Prints an array in a nicey-nice way */
void printarr(int* arr, int n) {
	int i;
	for(i = 0; i < n; i++) {
		printf("%d", arr[i]);
		if(i != n-1) printf(", ");
	}
	printf("\n");
}

/* Returns 1 if array is sorted, 0 otherwise */
int is_sorted(int* arr, int n) {
	int i;
	for(i = 1; i < n; i++)
		if(arr[i-1] > arr[i])
			return 0;
	return 1;
}

/* Feel free to modify this struct! */
typedef struct {
	int                tid;  /* thread ID */
	pthread_t*       thrds;  /* array of threads */
	int        num_threads;  /* length of threads array */
	int*               arr;  /* array to be sorted */
	int                  n;  /* length of array to be sorted */
	int* (*sort)(int*,int);  /* sorting algorithm */
} psortarg;

int* selection_sort(int* arr, int n) {

	int i, j, index;

	for(i = 0; i < n - 1; ++i){

		index = i;

		for(j = i + 1; j < n; ++j){

			if(arr[j] < arr[index]){

				index = j;

			}

		}

		int temp = arr[index];
		arr[index] = arr[i];
		arr[i] = temp;

	}
	
	return arr;

}

int* merge_sort(int* arr, int n) {

	if (n < 2) return arr;

	int m = n / 2;
	
	merge_sort(arr, m);
	merge_sort(arr + m, n - m);
	merge(arr, n, m);

}

void merge(int* a, int n, int m) {

    int i, j, k;

    int *x = malloc(n * sizeof (int));

    for (i = 0, j = m, k = 0; k < n; k++) {

        x[k] = j == n      ? a[i++]
             : i == m      ? a[j++]
             : a[j] < a[i] ? a[j++]
             :               a[i++];

    }

    for (i = 0; i < n; i++) {

        a[i] = x[i];

    }

    free(x);
}

int* quick_sort(int* arr, int n) {

	if(n < 2) return arr;

	int pivot = arr[n / 2];
	int i, j;

	for(i = 0, j = n - 1; ; ++i, --j){

		while(arr[i] < pivot) ++i;
		while(arr[j] > pivot) --j;

		if(i >= j) break; 

		int t = arr[i];
		arr[i] = arr[j];
		arr[j] = t;

	}	

	quick_sort(arr, i);
	quick_sort(arr + i, n - i);

}

void cleanupLock(void* arg){

	pthread_mutex_t* lock = (pthread_mutex_t*)arg;
	pthread_mutex_unlock(lock);

}

void* parallel_sort(void* _arg) {

	psortarg* arg = (psortarg*)_arg;
	
	/* Get local copy of arguments */
	int tid = arg->tid;
	pthread_t* thrds = arg->thrds;
	int num_threads = arg->num_threads;
	int* arr = arg->arr;
	int n = arg->n;

	/* Run sorting algorithm given to this thread */
	/* Ensure that the first algorithm to finish cancels all the others! */
	
	int status = pthread_once(&onceBlock, initMutexOnce);
	int os;

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &os);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &os);

	if(tid == 0){

		pthread_cleanup_push(cleanupLock, &m);
		pthread_cleanup_push(selection_sort, arg->sort);
		pthread_mutex_lock(&m);
		checkError(status);

		pthread_testcancel();

		status = pthread_mutex_unlock(&m);

		pthread_cleanup_pop(0);
		pthread_cleanup_pop(0);
		
		checkError(status);
		
		idWant = 0;

	} else if(tid == 1){

		pthread_cleanup_push(cleanupLock, &m);
		pthread_cleanup_push(merge_sort, arg->sort);
		pthread_mutex_lock(&m);
		checkError(status);

		pthread_testcancel();

		status = pthread_mutex_unlock(&m);

		pthread_cleanup_pop(0);
		pthread_cleanup_pop(0);
	
		checkError(status);

		idWant = 1;

	} else if(tid == 2){

		pthread_cleanup_push(cleanupLock, &m);
		pthread_cleanup_push(quick_sort, arg->sort);
		pthread_mutex_lock(&m);
		checkError(status);

		pthread_testcancel();

		status = pthread_mutex_unlock(&m);

		pthread_cleanup_pop(0);
		pthread_cleanup_pop(0);
	
		checkError(status);

		idWant = 2;
	
	}

	pthread_exit(NULL);

}

psortarg* newPsortArg(){

	psortarg *arg = (psortarg*)malloc(sizeof(psortarg));
	arg->tid = 0;
	arg->thrds = NULL;
	arg->num_threads = 0;
	arg->arr = NULL;
	arg->n = 0;
	arg->sort = NULL;

}

void freePsort(psortarg **args){

	int i;
	for(i = 0; i < 3; ++i){

		free(args[i]);

	}

	free(args);

}

int main(int argc, char* argv[]) {
	
	FILE* infile;
	int n, i;
	int* arr;
	int fastestalg = -1;
	
	/* Verify we have enough arguments */
	if(argc != 2) {
		printf("usage: ./psort <infile>\n");
		exit(0);
	}
	
	/* Read array from file */
	infile = fopen(argv[1], "r");
	fscanf(infile, "%d", &n);
	arr = (int*)malloc(n * sizeof(int));
	if(!arr) {
		printf("error: failed to malloc array\n");
		exit(10);
	}
	for(i = 0; i < n; i++)
		fscanf(infile, "%d", arr+i);
	fclose(infile);
	int copy[n];
	for(i = 0; i < n; ++i){

		copy[i] = arr[i];

	}
	
	/* Setup threads to run each sorting algorithm in parallel */
	/* Remember to ensure that the first algorithm to finish cancels all the others! */
	
	psortarg **args = malloc(3 * sizeof(psortarg));
	pthread_t threads[3];
	pthread_mutex_init(&m, NULL);
	int* sorts[3];
	sorts[0] = (*selection_sort)(arr, n);
	sorts[1] = (*merge_sort)(arr, n);
	sorts[2] = (*quick_sort)(arr, n);
	
	for(i = 0; i < 3; ++i){

		args[i] = newPsortArg();
		args[i]->tid = i;
		args[i]->thrds = threads;
		args[i]->num_threads = 3;
		args[i]->arr = arr;
		args[i]->n = n;
		args[i]->sort = sorts[i];

	}


	for(i = 0; i < 3; ++i){

		pthread_create(&threads[i], NULL, &parallel_sort, (void*)args[i]);

	}

	for(i = 0; i < 3; ++i){

		if(i > 0){

			pthread_cancel(threads[i]);

		} else {

			pthread_join(threads[i], NULL);

		}

	}

	/* Output final result */
	if(idWant == 0){

		printf("selection sort finished first!\n");

	} else if(idWant == 1){

		printf("merge sort finished first!\n");

	} else if(idWant == 2){

		printf("quick sort finished first!\n");

	}
	printf("in : ");
	printarr(copy, n);
	printf("out: ");
	printarr(arr, n);
	printf("sorting worked? %s\n", (is_sorted(arr,n) ? "yes" : "no"));
	
	/* Clean up! */
	free(arr);
	freePsort(args);
	pthread_mutex_destroy(&m);
	return 0;
}
