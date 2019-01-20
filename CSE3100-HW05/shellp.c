#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "shellp.h"

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *  Homework 7  *
 *  10/24/2017  *
 ***************/


char *strdup(const char *s);

// Check the bottom of this file to locate the  method you must implement.
// You do not need to read all provided code, but you are encouraged to
// do so if you want to experiment/understand more details about the program.

#define R_NONE 0    /* No redirections */
#define R_INP  1    /* input redirection bit */
#define R_OUTP 2    /* output redirection bit */
#define R_APPD 4    /* append redirection bit */

#define MAXRD 255

Stage* allocStage(int nba,char** args);
void freeStage(Stage*);
void printStage(Stage*);
Command* addCommandStage(Command* c,Stage* s);

int extractRedirect(char* buf,int* len,int* mode,char* input,char* output);

char* skipWS(char* arg);
char* cutWord(char* arg);
int trimString(char* buf,int len);

Command* allocCommand(char* c)
{
   Command* r = (Command*)calloc(1,sizeof(Command));
   r->_com = NULL;
   r->_kind = noCMD;
   r->_mode = R_NONE;
   return r;
}
void freeCommand(Command* c)
{
   if (c->_com) free(c->_com);
   if (c->_input) free(c->_input);
   if (c->_output) free(c->_output);
   for(int i=0;i < c->_nba;i++)
      if (c->_args[i]) free(c->_args[i]);
   free(c->_args);
   for(int j=0;j<c->_nbs;j++)
      freeStage(c->_stages[j]);
   if (c->_stages) free(c->_stages);
   free(c);
}

Command* setCommand(Command* c,enum Kind k,char* com)
{
   c->_kind = k;
   if (c->_com) free(c->_com);
   c->_com = strdup(com);
   return c;
}
Command* setCommandArgs(Command* c,int nb,char** args)
{
   c->_nba = nb + 1;
   c->_args = (char**)malloc(sizeof(char*)*c->_nba);
   for(int i=0;i<nb;i++)
      c->_args[i] = strdup(args[i]);
   c->_args[nb] = NULL;
   return c;
}

Command* addCommandStage(Command* c,Stage* s)
{
   printf("New stage:");
   printStage(s);

   c->_stages = realloc(c->_stages,sizeof(Stage*)*(c->_nbs + 1));
   c->_nbs += 1;
   c->_stages[c->_nbs - 1] = s;
   return c;
}

void printCommand(Command* c)
{
   if (c->_mode & R_INP)
      printf("<  [%s]\n",c->_input);
   if (c->_mode & R_OUTP)
      printf(">  [%s]\n",c->_output);
   if (c->_mode & R_APPD)
      printf(">> [%s]\n",c->_output);
   printf("CORE: %s\n",c->_com);
   for(int i=0;i<c->_nba;i++)
      printf("\targs[%d] = %s\n",i,c->_args[i]);
   printf("Stages:");
   for(int i=0;i < c->_nbs;i++)
      printStage(c->_stages[i]);
   printf("\n");
}


Command* makeCommand()
{
   char buffer[1024];
   int  i = 0;
   char ch,*ptr;
   printf("%%");fflush(stdout);
   while(i < sizeof(buffer) && (ch = getchar()) != '\n' && ch != EOF)
      buffer[i++] = ch;

   buffer[i] = 0;
   ptr = buffer+i-1;
   while(ptr>=buffer && isspace(*ptr)) ptr--,i--;
   *++ptr = 0;
   Command* c = allocCommand(ptr);
   if(ch==EOF)
      return setCommand(c,exitCMD,"exit");
   else {
      int mode = R_NONE;
      char input[1024];
      char output[1024];
      *input = *output = 0;
      int len = strlen(buffer);
      int ok = extractRedirect(buffer,&len,&mode,input,output);
      c->_mode = mode;
      c->_input = strdup(input);
      c->_output = strdup(output);
      len = trimString(buffer,len);
      if (ok) {
         char* sc = skipWS(buffer);
         char* ec = cutWord(sc);
         if (strcmp(sc,"cd")==0) {
            char* a0 = skipWS(ec);
            char* a1 = cutWord(a0);
            char* args[1] = {a0};
            return setCommandArgs(setCommand(c,cdCMD,sc),1,args);
         } else if (strcmp(sc,"pwd")==0) {
            return setCommand(c,pwdCMD,sc);
         } else if (strcmp(sc,"ln") == 0) {
            char* a0 = skipWS(ec);
            char* a1 = skipWS(cutWord(a0));
            char* a2 = cutWord(a1);
            char* args[2] = {a0,a1};
            return setCommandArgs(setCommand(c,linkCMD,sc),2,args);
         } else if (strcmp(sc,"rm") == 0) {
            char* a0 = skipWS(ec);
            char* a1 = cutWord(a0);
            char* args[1] = {a0};
            return setCommandArgs(setCommand(c,rmCMD,sc),1,args);
         } else if (strcmp(sc,"exit") == 0) {
            return setCommand(c,exitCMD,sc);
         } else {
            if (*sc) {
               if (strchr(ec,'|') != NULL) {
                  // This is a pipeline.
                  char*  args[512];
                  args[0] = sc;
                  char* arg = skipWS(ec);
                  int nba = 1;
                  setCommand(c,pipelineCMD,"");
                  while(arg && *arg) {
                     char* p = *arg == '|' ? arg : 0;
                     if (p) {
                        *p = 0;
                        addCommandStage(c,allocStage(nba,args));
                        args[0] = arg = skipWS(p+1);
                        arg = cutWord(arg);
                        nba = 1;
                     } else {
                        args[nba++] = arg;
                        arg = skipWS(cutWord(arg));
                     }
                  }
                  return addCommandStage(c,allocStage(nba,args));
               } else {
                  char*  args[1024];
                  args[0] = sc;
                  char* arg = ec;
                  int nba = 1;
                  while(arg && *arg) {
                     args[nba++] = arg;
                     arg = skipWS(cutWord(arg));
                     assert(nba < 1024);
                  }
                  return setCommandArgs(setCommand(c,basicCMD,args[0]),nba,args);
               }
            }
         }
      }
      return c;
   }
}

#define IS_REDIR(ch) ((ch) == '<' || (ch)=='>')

/* The routine extracts any redirects and replaces those parts of the command
   by whitespaces
   */

int extractRedirect(char* buf,int* len,int* mode,char* input,char* output)
{
   int i = 0;
   char* ptr = buf;
   while(ptr && *ptr) {
      if (IS_REDIR(*ptr)) {
         if (ptr[0]== '<') {
            if (*mode & R_INP) {
               printf("Ambiguous input redirect\n");
               return 0;
            } else {
               *ptr++ = ' ';
               i = 0;
               while(isspace(*ptr)) ptr++;
               while(isalnum(*ptr) || ispunct(*ptr)) {
                  if (i>=MAXRD) {
                     printf("redirect filename too long\n");
                     return 0;
                  }
                  input[i++] = *ptr;
                  *ptr++ = ' ';
               }
               input[i] = 0;
               *mode = *mode | R_INP;
            }
         } else if (ptr[0]=='>' && ptr[1]=='>') {
            if (*mode & (R_APPD | R_OUTP)) {
               printf("Ambiguous output redirect\n");
               return 0;
            } else {
               ptr[0] = ' ';
               ptr[1] = ' ';
               ptr += 2;
               i = 0;
               while(isspace(*ptr)) ptr++;
               while(isalnum(*ptr) || ispunct(*ptr)) {
                  if (i>=MAXRD) {
                     printf("redirect filename too long\n");
                     return 0;
                  }
                  output[i++] = *ptr;
                  *ptr++ = ' ';
               }
               output[i] = 0;
               *mode = *mode | R_APPD;
            }
         } else {
            if (*mode & (R_APPD | R_OUTP)) {
               printf("Ambiguous output redirect\n");
               return 0;
            } else {
               assert(ptr[0]=='>');
               *ptr++ = ' ';
               i = 0;
               while(isspace(*ptr)) ptr++;
               while(isalnum(*ptr) || ispunct(*ptr)) {
                  if (i>=MAXRD) {
                     printf("redirect filename too long\n");
                     return 0;
                  }
                  output[i++] = *ptr;
                  *ptr++ = ' ';
               }
               output[i] = 0;
               *mode = *mode | R_OUTP;
            }
         }
      }
      else ptr++;
   }
   return 1;
}


char* skipWS(char* arg)
{
   while(arg && *arg && isspace(*arg)) arg++;
   if(arg && *arg=='\"') arg++;
   return arg;
}

char* cutWord(char* arg)
{
   while(arg && *arg && !isspace(*arg)) arg++;
   if(*(arg-1)=='\"') arg--;
   if (arg && *arg) *arg++ = 0;
   return arg;
}

int trimString(char* buf,int len)
{
   char* ptr = buf + len - 1;
   while(ptr>=buf && isspace(*ptr))
      ptr--,len--;
   *++ptr = 0;
   return len;
}

// ================================================================================
// This part of the file contains the PipelineCommand object that you must modify
// ================================================================================
Stage* allocStage(int nba,char** args)
{
   Stage* s = (Stage*)calloc(1,sizeof(Stage));
   s->_nba = nba + 1;
   s->_args = (char**)calloc(s->_nba,sizeof(char*));
   for(int i=0;i<nba;i++)
      s->_args[i] = strdup(args[i]);
   return s;
}

void freeStage(Stage* s)
{
   for(int i=0;i<s->_nba;i++)
      if (s->_args[i] != NULL) free(s->_args[i]);
   free(s->_args);
   free(s);
}

void printStage(Stage* s)
{
   printf("\t(%d)[",s->_nba);
   for(int i=0;i<s->_nba;i++)
      if (s->_args[i] != NULL)
         printf("%s ",s->_args[i]);
      else printf("null ");
   printf("]\n");
}


// ================================================================================
// Write the pipelining logic here.
// ================================================================================

/***********************************
 * setUpCommandPipeline Function
 * This function sets up the piping
 * for the basicexecutes to function.
 * *********************************/

int setupCommandPipeline(Command* c){

	//Counters	
	int i = 0, j = 0;

	/*Piping of 2D needed because of the Stages** pointer in the Stage struct */
	int piping[c->_nbs - 1][2];

	/*Check if any piping failures */
	for(i = 0; i < c->_nbs - 1; i++){

		if(pipe(piping[i]) < 0){

			printf("Piping error.\n");
			return -1;

		}

	}

	/*Create forks for each command wanted to be executed */
	for(i = 0; i < c->_nbs; i++){

		/*Create parent and child process */
		pid_t p = fork();

		/*child process */
		if(p == 0){

			/*Make sure no errors in the input coming in to be put into pipeline are messed up */
			if(i != 0){

				/*Input error checking when piping is switched to input file location */
				if(dup2(piping[i-1][0], 0) < 0){

					printf("Error in input.\n");
					return -1;

				}

			}

			/*Make sure any potential output piping changes don't have errors */
			if(i != c->_nbs - 1){

				/*Output error checking whent he piping is switched to output file location */
				if(dup2(piping[i][1], 1) < 0){

					printf("Error in output.\n");
					return -1;

				}

			}

			/*Close read and write ends of one pipe */
			for(j = 0; j < c->_nbs - 1; j++){

				close(piping[j][0]);
				close(piping[j][1]);

			}

			/*first input from execution, basicExecute accordingly */
			if(i == 0 && R_INP == (c->_mode & R_INP)){

				basicExecute(c->_stages[i]->_args[0], R_INP, c->_input, NULL, c->_stages[i]->_args);

			/*If next command in the pipeline is output oreinted */
			} else if(i == c->_nbs - 1 && R_OUTP == (c->_mode & R_OUTP)){

				basicExecute(c->_stages[i]->_args[0], R_OUTP, NULL, c->_output, c->_stages[i]->_args);

			/*If next command in the pipeline is append oriented */
			} else if(i == c->_nbs - 1 && R_APPD == (c->_mode & R_APPD)){

				basicExecute(c->_stages[i]->_args[0], R_APPD, NULL, c->_output, c->_stages[i]->_args);

			/*If no command specified, do nothing but move to next command */
			} else {

				basicExecute(c->_stages[i]->_args[0], R_NONE, NULL, NULL, c->_stages[i]->_args);

			}

			/*Exit the child no more executing */
			abort();

		}

	}   

	/*Close all remaining pipes */
	for(i = 0; i < c->_nbs - 1; i++){

		close(piping[i][0]);
		close(piping[i][1]);

	}

	/*Wait for child process to end */
	while(waitpid(-1, NULL, 0) > 0);

	return 1;

}

// ================================================================================
// Write the basic command logic here.
// ================================================================================

/***********************************************
 * basicExecute Function
 * This function takes in arguments
 * for a basic shell and executes them toa file.
 * ********************************************/

int basicExecute(char* com,int mode,char* input,char* output,char** args)
{
  
	/*Fork into two processes */
   	pid_t p = fork();


	/*If there is an error while forking, return an error */
	if(p < 0){

		printf("Forking failed during execution\n");
		return -1;

	}

	/*Child process start */
	if(p == 0){


		/*If there needs input for the command to execute */
		if(R_INP == (mode & R_INP)){

			/*Create a new file to read the input, and switch input of process to the file location */
			FILE *newFile = fopen(input, "r");
			dup2(fileno(newFile), 0);
			fclose(newFile);

		}
	
		/*If output is needed for the command to execute */
		if(R_OUTP == (mode & R_OUTP)){

			/*Create a new file to write output on, and then switch the output file of the child to the file location */
			FILE *newFile = fopen(output, "w");
			dup2(fileno(newFile), 1);
			fclose(newFile);

		}

		/*If appending is needed for the command to execute */
		if(R_APPD == (mode & R_APPD)){

			/*Create a new file to append onto, and then switch the output file location with the file location */
			FILE *newFile = fopen(output, "a");
			dup2(fileno(newFile), 1);
			fclose(newFile);

		}

		/*Execute and upgrade to given command in to command pipeline */
		execvp(com, args);
		
		/*If executinf fails, send an error message */
		char *error = strerror(errno);
		printf("Error occured during execution: %s\n", error);
		return -1;

	/*Prent process */
	} else {
	
		/*Wait for the child process to finish and gut itself */
		while(waitpid(-1, NULL, 0) > 0);

   		return 1;

	}

}



int executeCommand(Command* c)
{
   switch(c->_kind) {
      case cdCMD: {
         int st = chdir(c->_args[0]);
         if (st==-1)
            printf("cd error: [%s]\n",strerror(errno));
         return TRUE;
      }break;
      case pwdCMD: {
         char buf[1024];
         char* ptr = getcwd(buf,sizeof(buf));
         if (ptr)
            printf("cwd: %s\n",buf);
         return TRUE;
      }break;
      case exitCMD:
         return FALSE;
      case linkCMD: {
         int rv = link(c->_args[0],c->_args[1]);
         if (rv!=0)
            printf("link error [%s]\n",strerror(errno));
         return TRUE;
      }break;
      case rmCMD: {
         int rv = unlink(c->_args[0]);
         if (rv!=0)
            printf("rm error: [%s]\n",strerror(errno));
         return TRUE;
      }break;
      case basicCMD: {
         return basicExecute(c->_com,c->_mode,c->_input,c->_output,c->_args);
      }break;
      case pipelineCMD: {
         setupCommandPipeline(c);
         return TRUE;
      }break;
      default:
         printf("oops....\n");
         return TRUE;
   }
}

int main(int argc,char* argv[]){

   int loop = 1;
   while(loop) {
      Command* com  = makeCommand();
      printCommand(com);
      loop = executeCommand(com);
      freeCommand(com);
   }
   return 0;
}


