#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "rwlock.h"

typedef struct {
	char* text;
} msg;

typedef struct {
	rwlock* lk;
	int index;
	msg* msg;
} argstruct;

static char* TEXT = "Hello world!";

/* Stolen from: https://stackoverflow.com/a/6852396 */
/* Used to generate random integers uniformly in the
 * range [0, max) without bias towards lower bits. 
 * Overkill? Yes. Needed? No. Do it anyway? Of course. */
long random_at_most(long max) {
	unsigned long
		num_bins = (unsigned long) max + 1,
		num_rand = (unsigned long) RAND_MAX + 1,
		bin_size = num_rand / num_bins,
		defect   = num_rand % num_bins;
	long x;
	do
		x = random();
	while (num_rand - defect <= (unsigned long)x);
	return x/bin_size;
}

void* writing(void* _arg) {
	argstruct* arg = (argstruct*) _arg;
	rwlock* lk = arg->lk;
	
	int i, len, st;
	while(1) {
		len = random_at_most(strlen(TEXT));
		printf("writer %d waiting...\n", arg->index); fflush(stdout);
		st = rwlock_wrlock(lk);
		if(st) {
			printf("writer %d: rwlock_wrlock error: %d\n", arg->index, st); fflush(stdout);
			exit(200);
		}
		printf("writer %d got lock! starting w/ len=%d!\n", arg->index, len); fflush(stdout);
		char* newmsg = (char*) malloc((len+1) * sizeof(char));
		if(!newmsg) exit(arg->index);
		for(i = 0; i < len; i++)
			newmsg[i] = TEXT[i];
		newmsg[len] = '\0';
		free(arg->msg->text);
		st = random_at_most(500)+500;
		printf("writer %d waiting %d milliseconds!\n", arg->index, st); fflush(stdout);
		usleep(1000*st);
		arg->msg->text = newmsg;
		printf("writer %d : new msg = %s\n", arg->index, arg->msg->text); fflush(stdout);
		printf("writer %d unlocking...\n", arg->index); fflush(stdout);
		st = rwlock_wrunlock(lk);
		if(st) {
			printf("writer %d: rwlock_wrunlock error: %d\n", arg->index, st); fflush(stdout);
			exit(201);
		}
		usleep(500*1000);
	}
	pthread_exit(NULL);
}

void* reading(void* _arg) {
	argstruct* arg = (argstruct*) _arg;
	rwlock* lk = arg->lk;
	int st;
	while(1) {
		printf("reader %d waiting...\n", arg->index); fflush(stdout);
		st = rwlock_rdlock(lk);
		if(st) {
			printf("reader %d: rwlock_rdlock error: %d\n", arg->index, st); fflush(stdout);
			exit(300);
		}
		printf("reader %d got lock!\n", arg->index); fflush(stdout);
		st = random_at_most(500)+500;
		printf("reader %d waiting %d milliseconds!\n", arg->index, st); fflush(stdout);
		usleep(1000*st);
		printf("reader %d : msg = %s\n", arg->index, arg->msg->text); fflush(stdout);
		printf("reader %d unlocking...\n", arg->index); fflush(stdout);
		st = rwlock_rdunlock(lk);
		if(st) {
			printf("reader %d: rwlock_rdunlock error: %d\n", arg->index, st); fflush(stdout);
			exit(301);
		}
		usleep(1000*random_at_most(750));
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	
	int i, st;
	int numreaders;
	int numwriters;
	
	/* Parse program args */
	if(argc != 3) {
		printf("usage: ./tester <#readers> <#writers>\n");
		return 0;
	}
	numreaders = atoi(argv[1]);
	numwriters = atoi(argv[2]);
	
	pthread_t rdrs[numreaders];
	pthread_t wtrs[numwriters];
	argstruct rdrargs[numreaders];
	argstruct wtrargs[numwriters];
	pthread_mutex_t mutex;
	
	rwlock lk;
	
	/* Initialize rwlock */
	if(st = rwlock_init(&lk)) {
		printf("lock init failed! err: %d\n", st);
		return 1000;
	}
	if(st = rwlock_destroy(&lk)) {
		printf("lock destroy failed! err: %d\n", st);
		return 1001;
	}
	if(st = rwlock_init(&lk)) {
		printf("lock init2 failed! err: %d\n", st);
		return 10002;
	}
	
	/* Initialize first message to be handled */
	msg* arg = (msg*) malloc(sizeof(msg));
	arg->text = (char*) malloc((strlen(TEXT)+1) * sizeof(char));
	for(i = 0; i < strlen(TEXT); i++)
		arg->text[i] = TEXT[i];
	arg->text[strlen(TEXT)] = '\0';
	
	/* Create reading threads */
	for(i = 0; i < numreaders; i++) {
		rdrargs[i].msg = arg;
		rdrargs[i].index = i;
		rdrargs[i].lk = &lk;
		st = pthread_create(&rdrs[i], NULL, reading, (void*)&rdrargs[i]);
		if(st) {
			printf("failed to create reader %d! err: %d\n", i, st);
			return 2;
		}
	}
	
	/* Create writing threads */
	for(i = 0; i < numwriters; i++) {
		wtrargs[i].msg = arg;
		wtrargs[i].index = i;
		wtrargs[i].lk = &lk;
		st = pthread_create(&wtrs[i], NULL, writing, (void*)&wtrargs[i]);
		if(st) {
			printf("failed to create writer %d! err: %d\n", i, st);
			return 3;
		}
	}
	
	/* This shouldn't happen if everything works. Will happen if threads crash */
	void** ret;
	for(i = 0; i < numreaders; i++) {
		st = pthread_join(rdrs[i], ret);
		if(st) {
			printf("error when joining reader threads. err: %d\n", st);
			return 4;
		}
	}
	for(i = 0; i < numwriters; i++) {
		st = pthread_join(wtrs[i], ret);
		if(st) {
			printf("error when joining writer threads. err: %d\n", st);
			return 5;
		}
	}
	
	/* Clean up! */
	free(arg->text);
	free(arg);
	
	/* Done! (if something broke, we'll get here) */
	return 0;
}
