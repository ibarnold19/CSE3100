#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include "data.h"

void checkError(int status)
{
   if (status < 0) {
      printf("socket error(%d): [%s]\n",getpid(),strerror(errno));
      exit(-1);
   }
}

int main(int argc,char* argv[])
{
   // Create a socket
   int sid = socket(PF_INET,SOCK_STREAM,0);
   // setup our address -- will listen on 8080 --
   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port   = htons(8090);
   addr.sin_addr.s_addr = INADDR_ANY;
   int status = bind(sid,(struct sockaddr*)&addr,sizeof(addr));
   checkError(status);
   status = listen(sid,10);
   checkError(status);

   while(1) {
      struct sockaddr_in client;
      socklen_t clientSize;
      int chatSocket = accept(sid,(struct sockaddr*)&client,&clientSize);
      checkError(chatSocket);
      printf("We accepted a socket: %d\n",chatSocket);
      pid_t child = fork();
      if (child == 0) {
         //int st = close(sid);
         FILE* f = fopen("/proc/loadavg","r");
         float l1,l5,l15;
         fscanf(f,"%f %f %f",&l1,&l5,&l15);
         fclose(f);
         l1 = htonf(l1);
         l5 = htonf(l5);
         l15 = htonf(l15);
         Load load = {l1,l5,l15};
         write(chatSocket,&load,sizeof(load));   
         close(chatSocket);
         return -1; /* Report that I died voluntarily */
      } else if (child > 0) {
         printf("Created a child: %d\n",child);
         close(chatSocket);
         int status = 0;
         pid_t deadChild; // reap the dead children (as long as we find some)
         do {
            deadChild = waitpid(0,&status,WNOHANG);checkError(deadChild);
            if (deadChild > 0)
               printf("Reaped %d\n",deadChild);
         } while (deadChild > 0);
      }
   }
   return 0;
}
