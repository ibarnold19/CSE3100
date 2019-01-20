#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define ITERATIONS 8
#define DEFAULT_NFEMALES 10
#define DEFAULT_NMALES 10
#define DEFAULT_NSTALLS 2

#define work() random_r(&buf, &random); usleep(random % 50000)
#define usebathroom() random_r(&buf, &random); usleep(random % 10000)

typedef struct bathroom {
   int    bfemales;        // number of females in the bathroom
   int    bmales;          // number of males in the bathroom
   int    nstalls;         // number of bathroom stalls
   pthread_mutex_t mutex;  // mutex to protect shared info
   pthread_cond_t  available;   // condition variable
} bathroom_t;

typedef struct thread_data {
   int id;
   bathroom_t* shared_info;
} thr_data;

void* female(thr_data* arg) {
   int i, id = arg->id;
   bathroom_t* shared_info = arg->shared_info;

   int32_t random;
   struct random_data buf;
   char state[64];
   initstate_r(id, state, sizeof(state), &buf);

   for(i=0; i<ITERATIONS; i++){
      work();
      pthread_mutex_lock(&shared_info->mutex);
      while(shared_info->bmales > 0) {  // must wait
         printf("female #%d waits\n", id);
         pthread_cond_wait(&shared_info->available, &shared_info->mutex);
      }
      shared_info->bfemales++;
      printf("female #%d enters bathroom\n", id);
      pthread_mutex_unlock(&shared_info->mutex);
      usebathroom();
      pthread_mutex_lock(&shared_info->mutex);
      shared_info->bfemales--;
      pthread_cond_broadcast(&shared_info->available);
      printf("female #%d exits bathroom\n", id);
      pthread_mutex_unlock(&shared_info->mutex);
   }
   pthread_exit(NULL);
}

void* male(thr_data* arg) {
   int i, id = arg->id;
   bathroom_t* shared_info = arg->shared_info;

   int32_t random;
   struct random_data buf;
   char state[64];
   initstate_r(id, state, sizeof(state), &buf);

   for(i=0; i<ITERATIONS; i++){
      work();
      pthread_mutex_lock(&shared_info->mutex);
      while(shared_info->bfemales > 0) {  // must wait
         printf("male #%d waits\n", id);
         pthread_cond_wait(&shared_info->available, &shared_info->mutex);
      }
      shared_info->bmales++;
      printf("male #%d enters bathroom\n", id);
      pthread_mutex_unlock(&shared_info->mutex);
      usebathroom();
      pthread_mutex_lock(&shared_info->mutex);
      shared_info->bmales--;
      pthread_cond_broadcast(&shared_info->available);
      printf("male #%d exits bathroom\n", id);
      pthread_mutex_unlock(&shared_info->mutex);
   }
   pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
   int i, n, nfemales = DEFAULT_NFEMALES, nmales = DEFAULT_NMALES, nstalls = DEFAULT_NSTALLS;

   for(i = 1; i < argc; i++) {
      if(strncmp(argv[i], "-f", strlen("-f")) == 0) {
         nfemales = atoi(argv[++i]);
      }
      else if(strncmp(argv[i], "-m", strlen("-m")) == 0) {
         nmales = atoi(argv[++i]);
      }
      else if(strncmp(argv[i], "-s", strlen("-s")) == 0) {
         nstalls = atoi(argv[++i]);
      }
      else {
         fprintf(stderr, "Usage: %s [-f N|-females N] [-m N|-males N] [-s N|-stalls N]\n", argv[0]);
         return 1;
      }
   }

   bathroom_t shared_info;
   shared_info.bfemales = shared_info.bmales = 0;
   shared_info.nstalls = nstalls;
   pthread_mutex_init(&shared_info.mutex, NULL);
   pthread_cond_init(&shared_info.available, NULL);

   n = nfemales + nmales;      // number of threads to create
   pthread_t tid[n];
   thr_data  data[n];

   for(i = 0; i < nfemales; i++) {
      data[i] = (thr_data){i,&shared_info};
      pthread_create(&tid[i], NULL, (void*(*)(void*))female, &data[i]);
   }
   for(i = nfemales; i < n; i++) {
      data[i] = (thr_data){i-nfemales,&shared_info};
      pthread_create(&tid[i], NULL, (void*(*)(void*))male, &data[i]);
   }
   for(i = 0; i < n; i++) {
      pthread_join(tid[i], NULL);
   }

   return 0;
}
