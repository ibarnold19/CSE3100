#include <unistd.h>
int main()
{
  int fd[2];
  pipe( fd );
  
   
  if( fork() == 0 ) {
        
     close(fd[0]);
     dup2(fd[1],1);
     close(fd[1]);
     
     execlp( "df", "df", "-h", NULL );
     
     
  } else {

     close(fd[1]);
     dup2(fd[0],0);
     close(fd[0]);
     
     execlp( "tail", "tail", "-n", "+2", NULL );
     
     
  }
}


