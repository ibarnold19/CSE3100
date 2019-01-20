#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

int computeInCircle(int n);

int main(int argc,char* argv[])
{
  int n = atoi(argv[1]);
  int k = atoi(argv[2]);
  printf("Running %d points with %d workers\n",n,k);
  int i,md = shm_open("/piz",O_CREAT|O_RDWR,S_IRWXU);
  ftruncate(md,4096);
  int* nbIn = mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,md,0);
  if (nbIn == MAP_FAILED) {
    printf("mmap failure! [%s]\n",strerror(errno));
    return 1;
  } else *nbIn = 0;
  sem_t* lock = sem_open("/pis",O_CREAT,0666,1);
  int nbDarts = n/k,rem = n % k,ttl = 0;
  for(i=0;i<k;i++) {
    int todo = (i < k-1) ? nbDarts : nbDarts + rem;
    ttl += todo;
    pid_t child = fork();
    if (child == 0) {
       int nbInCircle = computeInCircle(todo);
       sem_wait(lock);
       *nbIn += nbInCircle;
       sem_post(lock);
       sem_close(lock);      
       return 0;
    } 
  }
  for(i=0;i<k;i++) 
    wait(NULL);          
  int got = *nbIn;
  double pi = ((double)got)/n * 4.0;
  munmap(nbIn,4096);
  close(md);
  sem_close(lock);
  shm_unlink("/piz");
  sem_unlink("/pis");
  printf("Computation completed: %d - %d : PI = %lf\n",got,ttl,pi);
  return 0;
}

int computeInCircle(int n)
{
  unsigned short seed[3] = {0,0,0};
  seed48(seed);
  int i,in = 0;
  for(i=0;i< n;i++) {
    float x = erand48(seed);
    float y = erand48(seed);
    int  inCircle = (x*x+y*y) < 1.0;
    in += inCircle;
  }
  return in;
}

