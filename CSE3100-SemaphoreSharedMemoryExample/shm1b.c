#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

int main()
{
   const char* zone = "/memzone1";
   const char* sem  = "/sema2";

   int md = shm_open(zone,O_RDWR|O_CREAT,S_IRWXU);
   ftruncate(md,4096);

   void* ptr = mmap(NULL,4096,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED,
                    md,0); 
   if (ptr == MAP_FAILED) {
      printf("mmap failure: %s\n",strerror(errno));
      exit(1);
   }
   sem_t* s = sem_open(sem,O_CREAT,0666,1);
   memset(ptr,0,4096);
   int*  t = ptr;
   int i,v;
   for (i=0;i <10000000;i++) {
      //sem_wait(s);
     t[0] = t[0] + 1;
     //sem_post(s);
   }
   int final = t[0];
   munmap(ptr,4096);  
   close(md);
   shm_unlink(zone);
   sem_close(s);
   sem_unlink(sem);
   printf("Final: %d\n",final);
   return 0;
}