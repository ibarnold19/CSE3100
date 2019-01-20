#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

#include "command.h"

void checkError(int status)
{
   if (status < 0) {
      printf("socket error(%d): [%s]\n",getpid(),strerror(errno));
      exit(-1);
   }
}

void handleNewConnection(int chatSocket);

int main(int argc,char* argv[]) 
{
   // Create a socket
   int sid = socket(PF_INET,SOCK_STREAM,0);
   // setup our address -- will listen on 8080 --
   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   addr.sin_port   = htons(8080);
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
      int chatSocket = accept(sid,(struct sockaddr*)&client,&clientSize);
      checkError(chatSocket);
      printf("We accepted a socket: %d\n",chatSocket);
      pid_t child = fork();
      if (child == 0) {
         //int st = close(sid);
         handleNewConnection(chatSocket);
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

void handleNewConnection(int chatSocket)
{
   // This is the child process.
   int done = 0;
   do {
      Command c;
      int status = recv(chatSocket,&c,sizeof(Command),0);checkError(status);
      c.code = ntohl(c.code);
      switch(c.code){
         case CC_LS: {
            Payload p;
            char* msg = makeFileList(".");
            p.code = htonl(PL_TXT);
            p.length = htonl(strlen(msg)+1);
            status = send(chatSocket,&p,sizeof(Payload),0);checkError(status);
            int rem = strlen(msg)+1,sent = 0;
            while (rem != 0) {
               status = send(chatSocket,msg+sent,rem,0);
               rem -= status;
               sent += status;
            }
            free(msg);
         }break;
         case CC_GET: { // send the named file back to client			
            Payload p;
            int fileSize = getFileSize(c.arg);
            p.code = htonl(PL_FILE);
            p.length = htonl(fileSize);
            status = send(chatSocket,&p,sizeof(Payload),0);checkError(status);
            sendFileOverSocket(c.arg,chatSocket);
            printf("File [%s] sent\n",c.arg);
         }break; 
         case CC_PUT: {// save locally a named file sent by the client
            Payload p;
            status = recv(chatSocket,&p,sizeof(p),0);checkError(status);
            p.code = ntohl(p.code);
            p.length = ntohl(p.length);
            receiveFileOverSocket(chatSocket,c.arg,".upload",p.length);
            printf("File [%s] received\n",c.arg);
         }break;
         case CC_EXIT:
            done = 1;
            break;
      }
   } while (!done);
}
