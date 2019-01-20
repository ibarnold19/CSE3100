#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#include "data.h"

void checkError(int status,int line)
{
   if (status < 0) {
      printf("socket error(%d)-%d: [%s]\n",getpid(),line,strerror(errno));
      exit(-1);
   }
}

int main(int argc,char* argv[]) 
{
   if (argc < 2) {
      printf("usage is: rup <hostname>\n");
      return 1;
   }
   char* hostname = argv[1];
   int sid = socket(PF_INET,SOCK_STREAM,0);
   struct sockaddr_in srv;
   struct hostent *server = gethostbyname(hostname);
   srv.sin_family = AF_INET;
   srv.sin_port   = htons(8090);
   memcpy(&srv.sin_addr.s_addr,server->h_addr,server->h_length);
   int status = connect(sid,(struct sockaddr*)&srv,sizeof(srv));
   checkError(status,__LINE__);
   Load x;
   int nbr = 0;
   while (nbr != sizeof(Load))
      nbr += read(sid,((char*)&x)+nbr,sizeof(Load)-nbr);
   printf("load on: %s is %f\t %f\t %f\n",hostname,ntohf(x.l1),ntohf(x.l5),ntohf(x.l15));
   return 0;
}

