#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

/****************
 * Bryan Arnold *
 *  Homework 8  *
 *   CSE 3100   *
 *  12/6/2017   *
 ***************/

/*These globals are to make it easier to do certain
//computations so each function I implemented can keep
//track of. They include, t: the number of threads
//being used, count: the number of generations passed,
//generation: the default number of generations to do if
//no argument given, and a mutex for printing.*/
int t = 0;
int count = 0;
int generation = 1000;
pthread_mutex_t m;

/*The Board struct is the actual board
 * on which the Game of Life is played.
 * It contains the number of rows and
 * columns, as well as a 2D array for the board.*/
typedef struct BoardTag {
   
   int row;
   int col;
   char** src;

} Board;

/*The Barrier struct is the barrier between
 * the working threads to dictate when the threads
 * stop computing and to wait for ht eother one to catch
 * up. It includes a mutex and wait condition signal, the number
 * of threads being kept track by the barrier, the current number of
 * threads active, and the cycle.*/
typedef struct BarrierTag {

   pthread_mutex_t mutex;
   pthread_cond_t wait;
   int threads;
   int current;
   int cycle;

} Barrier;

/*The threadData struct is the data that each thread
 * needs to keep track of as it computes its portion of
 * the board during th eGame of Life.*/
typedef struct threadDataTag {

   Board *in;
   Board *out;
   int section, divide;
   int generation;

} threadData;

/*Gloabal barrier, just so it's easier for
//multiple fucntions to use and keep track of.*/
Barrier barr;

/*The makeBoard function initializes the components
 * inside the Board struct and returns a pointer to the struct.
 * The size of the Board is determined by other functions later on.*/
Board* makeBoard(int r,int c){

   Board* p = (Board*)malloc(sizeof(Board));
   p->row = r;
   p->col = c;
   p->src = (char**)malloc(sizeof(char*)*r);

   int i;
   for(i=0;i<r;i++)
      p->src[i] = (char*)malloc(sizeof(char)*c);

   return p;

}

/*The freeBoard function takes a pointer to a given
 * board and free the space allocated by the 2D array.*/
void freeBoard(Board* b){

   int i;
   for(i=0;i<b->row;i++)
      free(b->src[i]);

   free(b->src);
   free(b);

}

/*The readBoard function takes a text file
 * and maps onto a 2D array the locations of the '*' character
 * and the boarder of the board as '|'. It creates a new board
 * based on what was read and then returns a pointer to that Board.*/
Board* readBoard(char* fName){

   int row,col,i,j;
   FILE* src = fopen(fName,"r");
   fscanf(src,"%d %d\n",&row,&col);
   Board* rv = makeBoard(row,col);

   for(i=0;i<row;i++) {

      for(j=0;j<col;j++) {

         char ch = fgetc(src);
         rv->src[i][j] = ch == '*';
      
      }

      char skip = fgetc(src);
      while (skip != '\n') skip = fgetc(src);
   
   }
   
   fclose(src);   
   return rv;

}
/*The saveBoard function takes a given board,
 * and all of the '*' positions within in, and
 * puts them into a new text file.*/
void saveBoard(Board* b,FILE* fd){

   int i,j;
   for(i=0;i<b->row;i++) {

      fprintf(fd,"|");

      for(j=0;j < b->col;j++) 
         fprintf(fd,"%c",b->src[i][j] ? '*' : ' ');

      fprintf(fd,"|\n");

   }

}

/*The clearScreen function simply clears the standard
 * output screen for the user cleared of all output.*/
void clearScreen(){

   static const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
   static int l = 10;
   write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, l);

}

/*The printBoard function prints out a given board
 * onto the standard output of the user.*/
void printBoard(Board* b){

   clearScreen();	
   saveBoard(b,stdout);

}

/*The liveNeighbors function takes a look around a certain
 * position of a '*' in the board and counts the nieghboring ones.
 * It them returns the number of neighbors that are alive next to the
 * position in questions.*/
int liveNeighbors(int i,int j,Board* b){

   const int pc = (j-1) < 0 ? b->col-1 : j - 1;
   const int nc = (j + 1) % b->col;
   const int pr = (i-1) < 0 ? b->row-1 : i - 1;
   const int nr = (i + 1) % b->row;

   int xd[8] = {pc , j , nc,pc, nc, pc , j , nc };
   int yd[8] = {pr , pr, pr,i , i , nr , nr ,nr };

   int ttl = 0;
   int k;

   for(k=0;k < 8;k++)
      ttl += b->src[yd[k]][xd[k]];
   
   return ttl;

}

/*The evolveBoard function takes two boards
 * and then transforms the first board given into the
 * second one so there are now two copies of the original
 * board. Basically, changes how the board looks after neighbors 
 * are counted.*/
void evolveBoard(Board* src,Board* out){

   static int rule[2][9] = {
      {0,0,0,1,0,0,0,0,0},
      {0,0,1,1,0,0,0,0,0}
   };

   int i,j;
   for(i=0;i<src->row;i++) {

      for(j=0;j<src->col;j++) {

         int ln = liveNeighbors(i,j,src);
         int c  = src->src[i][j];
         out->src[i][j] = rule[c][ln];
      
      }

   }

}

/*The update method looks at the board after all of the threads
 * are done looking at the board in question and updating it,
 * then combines all of the changes into one board. This is the
 * generation iterator essentially.*/
void* update(Board *now, Board *next, int height, int part){

   int i, j;
   for(i = part; i < (part + height); i++){

	for(j = 0; j < now->col; j++){

		now->src[i][j] = next->src[i][j];

	}

   }

}

/*The newBarrier function creates a new Barrier struct
 * and returns a pointer to it. It needs to create the mutex, condition signal,
 * the current threads needed to run is also needed for this function.
 * Cycle is default 0.*/
void newBarrier(Barrier *barrier, int current){

   barrier->threads = barrier->current = current;
   barrier->cycle = 0;
   pthread_mutex_init(&barrier->mutex, NULL);
   pthread_cond_init(&barrier->wait, NULL);

}

/*The destroyBarrier function gets rid of
 * all the components of the Barrier struct being sent
 * to the function that are no longer needed. So, it 
 * destroys the condition signal as well as the mutex.*/
void destroyBarrier(Barrier *barrier){

   pthread_mutex_destroy(&barrier->mutex);
   pthread_cond_destroy(&barrier->wait);

}

/*The waitBarrier function waits for each thread that is part
 * of the barrier to reach the barrier, then releases all the threads to continue computations,
 * Basically, it holds threads back until each thread is done a certain process,
 * then allows all of them to proceed.*/
void waitBarrier(Barrier *barrier){

   /*Create some local variables for tracking things,
   as well as lock the given barrier's mutex*/
   int status, stop, temp, cycle;
   pthread_mutex_lock(&barrier->mutex);

   cycle = barrier->cycle;

   /*If the last thread has reach the barrier.*/
   if(--barrier->current == 0){

        /*Release the threads and broadcast the
 	barrier's condition.*/
	barrier->cycle = !barrier->cycle;
	barrier->current = barrier->threads;
	pthread_cond_broadcast(&barrier->wait);

   /*IF the thread entering isn't the last thread to reach the barrier.*/
   } else {

	/*Wait with cancellation disabled, because this function
 	* shouldn't be a cancellation point. Prevents threads from ceasing
 	* execution.*/
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &stop);

	/*Wait for the barrier's cycle to change, meaning the condition
 	* has been broadcast, so no more waiting is needed.*/
	while(cycle == barrier->cycle){

		/*IF condition signaled, break.*/
		status = pthread_cond_wait(&barrier->wait, &barrier->mutex);
		if(status != 0){

			break;

		}

	}

	/*Set cancel state back.*/
	pthread_setcancelstate(stop, &temp);

   }

   /*Release threads.*/
   pthread_mutex_unlock(&barrier->mutex);

}

/*The threadData function allocs space
 * for the data that a thread will need to store as
 * well as setting the other components to a default
 * of 0 or NULL.*/
threadData* newThreadData(){

   threadData *data = (threadData*)malloc(sizeof(threadData));
   data->in = NULL;
   data->out = NULL;
   data->section = 0;
   data->divide = 0;
   
   return data;

}

/*The freeData function takes in a @D array of threadData 
 * and free what each thread has data wise as well as freeing
 * the threadData struct itself.*/
void freeData(threadData **data){

	int i;
	for(i = 0; i < t; i++){

		free(data[i]);

	}

	free(data);

}

/*The play function takes a group of arguments and
 * does out the process that each thread must carry out
 * in order for the Game of Life to be successful.*/
void* play(void* args){

   /*Create new pointer to thread data given in the play call in main.*/
   threadData *data = (threadData*)args;

   /*Create two new pointers to boards from the given data.*/
   Board *now = data->in;
   Board *next = data->out;

   /*Create local variables of the dimensions of the
 * board in question, as well as the section each thread
 * takes and the divide of threads.*/
   int rows = now->row;
   int cols = now->col;
   int section = data->section;
   int divide = data->divide;
   int i = 0;

   /*Height of the board as wel as the part size each thread computes.*/
   int height = (int)(rows / divide);
   int part = height * section;

   while(i < generation){

	/*Change the board parts for each thread, then wait for each thread to finish.*/
	evolveBoard(now, next);
	waitBarrier(&barr);

	/*Update the board based on the results from the threads, then update the count of generations
 	* processed so far.*/
	update(now, next, height, part);
	count++;

	/*Use a mutex to lock out access to printing, this just prevents multiple prints at the same time.*/
	pthread_mutex_lock(&m);

	/*If the number of generations is divisble by the number of threads
 	to divide the board, print it out.*/
	if(count % divide == 0){

		printBoard(next);
		usleep(100000);

	}

	/*Unlock board, then wait one more time
 	* for any thread printing, and increment i.*/
	pthread_mutex_unlock(&m);
	waitBarrier(&barr);
	i++;

   }

}

/*Main Function.*/
int main(int argc,char* argv[])
{

   /*Makes sure that the given arguments to run the program are correct.*/
   if(argc < 2 || argc > 3){

	printf("Correct usage: ./lifeMT <file.txt> or ./lifeMT <file.txt> numGenerations\n");
	return 1;

   }

   /*Create two locak boards and pointers to them to
   * be the first boards of the process. Also, create local
   * variable for generations based on the gloab default.*/
   Board* life1 = readBoard(argv[1]);
   Board* life2 = makeBoard(life1->row,life1->col);
   int g = generation;

   /*Depending on the board dimensions, the number of threads 
   *is even for even rows, and odd for odd rows.*/ 
   if(life1->row % 2 == 0){

	t = 2;

   } else {

	t = 3;

   }

   /*If the number of generations was specified, change the local
   *variable g to the argument desired, then change the global
   to the same value.*/ 
   if(argc == 3){

	g = atoi(argv[2]);

   } 

   generation = g;

   /*Create new barrier for use based on global barrier,
   *create a new mutex based on global mutex, initialize
   new collection of thread data and threads based on the
   number of needed threads t (the global).*/ 
   int i;
   newBarrier(&barr, t);
   pthread_mutex_init(&m, NULL);
   threadData **data = malloc(t * sizeof(threadData));
   pthread_t threads[t];

  /*Initialize appropriate number of threadData structs
 * for each thread and then initialize them to the inital boards,
 * the number of threads, section, generations, etc.*/
  for(i = 0; i < t; i++){

	data[i] = newThreadData();
	data[i]->in = life1;
	data[i]->out = life2;
	data[i]->section = i;
	data[i]->divide = t;
	data[i]->generation = g;

  }

  /*Make each thread run the play function on the board up to the
 * global generation number with each threads given thread data.*/
  for(i = 0; i < t; i++){

	pthread_create(&threads[i], NULL, &play, (void*)data[i]);

  }

  /*After the threads successfully compute the play function,
 * combine them back together for the result.*/
  for(i = 0; i < t; i++){

	pthread_join(threads[i], NULL);

  }

   /*Now, simply just open a new file, save the resulting board
 * into that file, close the file, free each board, destroy the
 * barrier being used, free each threadDAta, and destroy the mutex being made.*/
   FILE* final = fopen("finalMT.txt","w");
   saveBoard(life2,final);
   fclose(final);
   freeBoard(life1);
   freeBoard(life2);
   destroyBarrier(&barr);
   freeData(data);
   pthread_mutex_destroy(&m);

}
