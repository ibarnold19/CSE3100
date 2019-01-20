#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char* readResponse(int fd);

int main(){

  int fd[2];
   
  int p = fork();

  if(pipe(fd) == -1){

	printf("Piping failed\n");
	exit(1);

  }

  if(p < 0){

	printf("Error forking\n");
	exit(1);

  } else if(p == 0){
        
     close(fd[1]);

     close(0);
     dup(fd[0]);
     close(fd[0]);
 
     execlp("tail", "tail", "-n", "+2", NULL); 
 
     
  } else {

     close(fd[0]);
	
     wait(NULL);

     close(fd[1]);

     execlp("df", "df", "-h", NULL);
    
  }

}

