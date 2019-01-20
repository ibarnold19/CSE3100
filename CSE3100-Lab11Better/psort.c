#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

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
	int*        done_first;  /* thread ID of first finisher */
	
	pthread_barrier_t* bar;  /* barrier to ensure threads all start at the same time */
	pthread_mutex_t*   mut;  /* mutex to ensure only one thread can cancel the others */
} psortarg;

void swap(int* xp, int* yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

/* Implementation stolen from:
 * http://www.geeksforgeeks.org/selection-sort/ */
int* selection_sort(int* arr, int n) {
	int i, j, min_idx;
	for(i = 0; i < n-1; i++) {
		min_idx = i;
		for(j = i+1; j < n; j++) {
			pthread_testcancel();
			if(arr[j] < arr[min_idx])
				min_idx = j;
		}
		swap(arr+min_idx, arr+i);
	}
	return arr;
}

/* Implementation stolen from:
 * http://www.geeksforgeeks.org/merge-sort/ */
void merge(int* arr, int l, int m, int r) {
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;
	int L[n1], R[n2];
	for(i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for(j = 0; j < n2; j++)
		R[j] = arr[m + j + 1];
	i = j = 0;
	k = l;
	while(i < n1 && j < n2) {
		if(L[i] <= R[j]) {
			arr[k] = L[i];
			i++;
		}else{
			arr[k] = R[j];
			j++;
		}
		k++;
	}
	while(i < n1) {
		arr[k] = L[i];
		i++;
		k++;
	}
	while(j < n2) {
		arr[k] = R[j];
		j++;
		k++;
	}
}
void merge_sort_helper(int* arr, int l, int r) {
	if(l < r) {
		pthread_testcancel();
		int m = l + (r-l) / 2;
		merge_sort_helper(arr, l, m);
		merge_sort_helper(arr, m+1, r);
		merge(arr, l, m, r);
	}
}
int* merge_sort(int* arr, int n) {
	merge_sort_helper(arr, 0, n-1);
	return arr;
}

/* Implementation stolen from:
 * http://www.geeksforgeeks.org/quick-sort/ */
int partition(int* arr, int low, int high) {
	int pivot = arr[high];
	int i = low - 1, j;
	for(j = low; j < high; j++) {
		pthread_testcancel();
		if(arr[j] <= pivot) {
			i++;
			swap(arr+i, arr+j);
		}
	}
	swap(arr+i+1, arr+high);
	return i+1;
}
void quick_sort_helper(int* arr, int low, int high) {
	if(low < high) {
		int pi = partition(arr, low, high);
		quick_sort_helper(arr, low, pi-1);
		quick_sort_helper(arr, pi+1, high);
	}
}
int* quick_sort(int* arr, int n) {
	quick_sort_helper(arr, 0, n-1);
	return arr;
}

void cleanup(void* _arg) {
	int* arg = (int*)_arg;
	free(arg);
}

void* parallel_sort(void* _arg) {
	psortarg* arg = (psortarg*)_arg;
	int i;
	
	/* Get local copy of arguments */
	int tid = arg->tid;
	pthread_t* thrds = arg->thrds;
	int num_threads = arg->num_threads;
	int* arr = arg->arr;
	int n = arg->n;
	int* (*sort)(int*,int) = arg->sort;
	pthread_barrier_t* bar = arg->bar;
	pthread_mutex_t* mut = arg->mut;
	
	/* Run sorting algorithm given to this thread */
	/* Ensure that the first algorithm to finish cancels all the others! */
	
	/* Make a copy of the array */
	int* newarr = (int*)malloc(n * sizeof(int));
	if(!newarr) {
		printf("failed to malloc array copy in thread %d!\n", tid);
		exit(200);
	}
	for(i = 0; i < n; i++)
		newarr[i] = arr[i];
	
	/* Wait for all threads to be ready... */
	pthread_barrier_wait(bar);
	
	/* Go! */
	pthread_cleanup_push(cleanup, (void*)newarr);
	(*sort)(newarr, n);
	pthread_cleanup_pop(0);
	
	/* If we're the first one done, get mutex and
	 * test done_first flag! */
	if(!pthread_mutex_trylock(mut) && *arg->done_first == -1) {
		*arg->done_first = tid;
		for(i = 0; i < num_threads; i++) {
			if(tid != i)
				pthread_cancel(thrds[i]);
		}
		for(i = 0; i < n; i++)
			arr[i] = newarr[i];
		pthread_mutex_unlock(mut);
	}
	
	/* Clean up! */
	free(newarr);
	
	/* Done! */
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	
	FILE* infile;
	int n, i;
	int* arr;
	int* arrcpy;
	int fastestalg = -1;
	
	/* Verify we have enough arguments */
	if(argc != 2) {
		printf("usage: ./psort <infile>\n");
		exit(0);
	}
	
	/* Read array from file */
	infile = fopen(argv[1], "r");
	fscanf(infile, "%d", &n);
	arrcpy = (int*)malloc(n * sizeof(int));
	arr = (int*)malloc(n * sizeof(int));
	if(!arr || !arrcpy) {
		printf("error: failed to malloc array\n");
		exit(10);
	}
	for(i = 0; i < n; i++) {
		fscanf(infile, "%d", arr+i);
		arrcpy[i] = arr[i];
	}
	fclose(infile);
	
	/* Setup threads to run each sorting algorithm in parallel */
	/* Remember to ensure that the first algorithm to finish cancels all the others! */
	pthread_t thrds[3];
	psortarg args[3];
	int* (*sort[3])(int*,int) = { selection_sort, merge_sort, quick_sort };
	pthread_barrier_t bar;
	if(pthread_barrier_init(&bar, NULL, 3)) {
		printf("failed to initialize barrier!\n");
		exit(100);
	}
	pthread_mutex_t mut;
	if(pthread_mutex_init(&mut, NULL)) {
		printf("failed to initialize mutex!\n");
		exit(110);
	}
	int done_first = -1;  /* used to track who finished first! */
	for(i = 0; i < 3; i++) {
		args[i].tid = i;
		args[i].thrds = thrds;
		args[i].num_threads = 3;
		args[i].arr = arr;
		args[i].n = n;
		args[i].sort = sort[i];
		args[i].bar = &bar;
		args[i].mut = &mut;
		args[i].done_first = &done_first;
		if(pthread_create(&thrds[i], NULL, parallel_sort, (void*)&args[i])) {
			printf("failed to create thread %d!\n", i);
			exit(400);
		}
	}
	void* ret;
	for(i = 0; i < 3; i++)
		if(pthread_join(thrds[i], &ret)) {
			printf("failed to join thread %d!\n", n);
			exit(500);
		}
	
	/* Throw results in standard output */
	if(done_first == 0) printf("selection sort");
	else if(done_first == 1) printf("merge sort");
	else if(done_first == 2) printf("quick sort");
	printf(" finished first!\n");
	printf("in : ");
	printarr(arrcpy, n);
	printf("out: ");
	printarr(arr, n);
	printf("is sorted? %s\n", (is_sorted(arr,n) ? "yes" : "no"));
	
	/* Clean up! */
	pthread_barrier_destroy(&bar);
	pthread_mutex_destroy(&mut);
	free(arr);
	
	/* Done! */
	return 0;
}
