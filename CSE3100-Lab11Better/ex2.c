#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void checkError(int st) {
	if(st) abort();
}

pthread_once_t onceBlock = PTHREAD_ONCE_INIT;
pthread_mutex_t mutex;

void initMutexOnce(void) {
	int st = pthread_mutex_init(&mutex, NULL);
	if(st) exit(-1);
}

void* minionDoWork(void* arg) {
	int status = pthread_once(&onceBlock, initMutexOnce);
	int os;
	
	/* "Look busy." Need to disable cancellation temporarily 
	 * as sleep contains a predefined cancellation point. */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &os);
	sleep(1);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &os);
	
	/* Now do the dangerous stuff. */
	status = pthread_mutex_lock(&mutex);
	checkError(status);
	printf("We are in the critical section [%ld]!\n", (long)arg);
	usleep(500*1000);
	
	/* Let's see if anyone is trying to cancel us... */
	pthread_testcancel();
	
	/* Get out of critical section */
	status = pthread_mutex_unlock(&mutex);
	
	checkError(status);
	return 0;
}

int main() {
	pthread_t tid[20];
	int status;
	for(long k = 0; k < 20; k++) {
		status = pthread_create(tid+k, NULL, minionDoWork, (void*)k);
		checkError(status);
	}
	pthread_cancel(tid[4]);
	for(int k = 0; k < 20; k++) {
		pthread_join(tid[k], NULL);
	}
	return 0;
}
