#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <dirent.h>

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
   // setup our address -- will listen on 8025 --
   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port   = htons(8025);
   addr.sin_addr.s_addr = INADDR_ANY;
   //pairs the newly created socket with the requested address.
   int status = bind(sid,(struct sockaddr*)&addr,sizeof(addr));
   checkError(status);
   // listen on that socket for "Let's talk" message. No more than 10 pending at once
   status = listen(sid,10);
   checkError(status);

   while(1) {
      struct sockaddr_in client;
      socklen_t clientSize;
      int rshSocket = accept(sid,(struct sockaddr*)&client,&clientSize);
      checkError(rshSocket);
      printf("We accepted a socket: %d\n",rshSocket);
      pid_t child = fork();
      if (child == 0) {
	dup2(rshSocket,0);
	dup2(rshSocket,1);
	dup2(rshSocket,2);
	close(rshSocket);
	int error = execlp("/bin/bash","/bin/bash","-l",NULL);
	checkError(error);
	return -1; /* Report that I died voluntarily */
      } else if (child > 0) {
         printf("Created a child: %d\n",child);
         close(rshSocket);		
         int status = 0;
         pid_t deadChild; // reap the dead children (as long as we find some)
         do {
            deadChild = waitpid(0,&status,WNOHANG);
	    checkError(deadChild);
            if (deadChild > 0)
               printf("Reaped %d\n",deadChild);
         } while (deadChild > 0); 
      } 
   }
   return 0;
}
