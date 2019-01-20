#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

int main()
{
   const char* zone = "/memzone1";
   int md = shm_open(zone,O_RDWR,S_IRWXU);
   void* ptr = mmap(NULL,4096,
                    PROT_READ|PROT_WRITE,
                    MAP_SHARED|MAP_FILE,
                    md,0);
   sem_t* s = sem_open("/sema2",O_EXCL);
   int*  t = ptr;
   int i;
   for (i=0;i <1000000;i++) {
     sem_wait(s);
     t[0] = t[0] + 1;
     sem_post(s);
   }
   int final = t[0];
   close(md);
   sem_close(s);
   munmap(ptr,4096);  
   printf("Final: %d\n",final);
   return 0;
}