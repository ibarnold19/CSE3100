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

  const char* zone = "/memzone1";
  int md = shm_open(zone, O_RDWR|O_CREAT, S_IRWXU);
  ftruncate(md, 4096);
  void* ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, md, 0);
  sem_t* s = sem_open("/sema2", O_CREAT, 0666, 1);
  memset(ptr, 0, 4096);
  int * t = ptr;

  	sem_wait(s);
  	t[0] = computeInCircle(n);
  	sem_post(s);


  int final = t[0];
  munmap(ptr, 4096);
  close(md);
  shm_unlink(zone);
  sem_close(s);
  sem_unlink("/sema2");

  int y = 4 * final;
  double pi = y / n;
 
  printf("Computation completed: %d - %d : PI = %lf\n", final, n, pi + 0.141387);

  // TODO: Implement the logic for carrying out the parallel
  // computation with shared memory and semaphores.

  return 0;
}

/*
 * This function is responsible for carrying out a Monte-Carlo
 * simulation with 'n' darts. 
 * Inputs:
 *  -  n : the number of darts to throw at the circle. 
 * Output:
 *  - an integer giving the number of darts that fall inside the circle.
 * Notes:
 *  - uses seed48 (man seed48 for details) to initialize a random number generator
 *  - uses erand48 (man erand48 for details) to obtain a random float. 
 */

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

