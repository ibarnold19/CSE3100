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
   // setup our address -- will listen on 8090 --
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
	int rupSocket = accept(sid, (struct sockaddr*)&client, &clientSize);
	checkError(rupSocket);

	printf("We accepted a socket: %d\n", rupSocket);
	pid_t child = fork();

	if(child == 0){

		dup2(rupSocket, 0);
		dup2(rupSocket, 1);
		dup2(rupSocket, 2);
		close(rupSocket);
	
		int error = execlp("cat", "cat", "/proc/loadavg", NULL);
		checkError(error);
		return -1;

	} else if (child > 0){

		printf("Child created: %d\n", child);
		close(rupSocket);
		int status = 0;
		pid_t deadChild;

		do{
		
			deadChild = waitpid(0, &status, WNOHANG);
			checkError(deadChild);

			if(deadChild > 0) printf("Reaped %d\n", deadChild);			

		}while(deadChild > 0);

	}

      // Todo: Implement the sever logic here.
      
   }
   return 0;
}
