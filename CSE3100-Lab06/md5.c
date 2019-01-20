#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
char* md5sum(char* payload,size_t sz);

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *    Lab 6     *
 *  10/21/2017  *
 ***************/

int main(int argc,char* argv[])
{

   /*Make sure number of arguments is correct*/
   if(argc != 2){

	printf("usage: ./md5 filename\n");
	return 1;

   }

   char* fName = argv[1];
   FILE* src = fopen(fName,"r");
   fseek(src,0,SEEK_END);
   long sz = ftell(src);
   fseek(src,0,SEEK_SET);
   char* payload = malloc(sizeof(char)*sz);
   fread(payload,sizeof(char),sz,src);
   fclose(src);
   char* rv = md5sum(payload,sz);
   printf("Got md5 [%s]\n",rv);
   free(payload);
   free(rv);
   return 0;
}

void sendPayload(int fd,char* payload,size_t sz) {
   long sent=0,rem = sz;
   while(sent != sz) {
      int nbSent = write(fd,payload+sent,rem);
      sent += nbSent;
      rem  -= nbSent;
   }
	
}

char* readResponse(int fd) {
   int sz = 8;
   char* buf = malloc(sz);
   int ttl = 0,at = 0;
   int recvd;
   do {
      recvd = read(fd,buf+at,sz - at);
      ttl += recvd;
      at  += recvd;
      if (recvd > 0 && ttl == sz) {
         buf = realloc(buf,sz*2);
         sz *= 2;

      }
   } while (recvd >0 );
   char* final = malloc(ttl+1);
   memcpy(final,buf,ttl);
   final[ttl] = 0;
   free(buf);
   return final;
}
// Implement your solution here
//   /*
//   1. Forking child process
//   2. Bi-directional pipes w/ appropriate std i/o
//   3. child exec md5Sum and upgrades
//   4. parent sends content of file (given from command arg) to child and gets back md5Sum
char* md5sum(char* payload,size_t sz){

	/*Declaration of two pipes */
	int parentOutChildIn[2];
	int parentInChildOut[2];

	pid_t p; //Fork id numbers

	//If piping for first pipe fails
	if(pipe(parentOutChildIn) == -1){

		printf("Piping failed.\n");
		exit(1);

	}

	//If piping for second pipe fails
	if(pipe(parentInChildOut) == -1){

		printf("Piping failed.\n");
		exit(1);

	}

	//Split into two prophecies
	p = fork();	
	
	//If forking fails
	if(p < 0){

		printf("Forking failed.\n");
		exit(1);
	
	  /*The parent process */
	} else if(p > 0){

		//Close read side of first pipe
		close(parentOutChildIn[0]);

		//Send payload to the write side of the first pipe for the parent process
		sendPayload(parentOutChildIn[1], payload, sz + 1);
		//Close the write side of the first pip for the parent process
		close(parentOutChildIn[1]);		

		//Wait for child to return
		wait(NULL);

		//Close write side of the second pipe for the parent process
		close(parentInChildOut[1]);

		//The hash returned from the execlp from the child process into read side of second pipe
		char* response = readResponse(parentInChildOut[0]);
		//Close the read side of the second pipe for the parent process
		close(parentInChildOut[0]);		

		//Not sure on what the output was supposed to look like since
		//both cat and md5sum produced different outputs in the
		//console, so I chose this.
		response[34] = '-';
		response[35] = '\n';
		response[36] = '\0';

		//get rid of temporary file
		remove("newFile.txt");

		//Return the hash to main
		return response;	

	} else {

		//Close the write side of the pipe for the child process
		close(parentOutChildIn[1]);

		//Read the payload sent from the parent into the read side of the second pipe for the child process
		char* toHash = readResponse(parentOutChildIn[0]);		

		long size = 0;

		//Find size of payload, just to add string indicator to the end of it
		while(toHash[size] != '\0'){

			size++;

		}

		toHash[size] = '\0';

		//Close stdin and dup it to the read side of the first pipe for child
		close(0);
		dup(parentOutChildIn[0]);
		//Close the read side of the first pipe for the child
		close(parentOutChildIn[0]);
		//Close the read side of the second pipe
		close(parentInChildOut[0]);
		//Close stdin and dup it to the write side of the second pipe for child
		close(1);
		dup(parentInChildOut[1]);
		//Close the write side of the second pipe for the child
		close(parentInChildOut[1]);

		//Open new temporary file to write the text to hash and put the string to hash into it
		FILE *file = fopen("newFile.txt", "w");
		fprintf(file, &toHash[0]);
		fclose(file);

		//Exec the child process and upgrade it to md5sum
		execlp("md5sum", "md5sum", "newFile.txt", NULL);

	}

}
