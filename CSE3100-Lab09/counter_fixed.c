#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int x = 0;
pthread_mutex_t m;

void increase(int cnt) {
	int i;
	for(i=0;i<cnt;i++) {
		pthread_mutex_lock(&m);
		x = x + 1;
		pthread_mutex_unlock(&m);
	}
}

int main(int argc,char* argv[]) {
	pthread_t tid1,tid2;
	int status;
	long cnt = atol(argv[1]);
	pthread_mutex_init(&m,NULL);
	
	status = pthread_create(&tid1, NULL, (void*(*)(void*))increase, (void*)cnt);
	status = pthread_create(&tid2, NULL, (void*(*)(void*))increase, (void*)cnt);
	
	void *v1,*v2;
	pthread_join(tid1,&v1);	
	pthread_join(tid2,&v2);
	pthread_mutex_destroy(&m);
	printf("counter is %d\n",x);
	return 0;
}
