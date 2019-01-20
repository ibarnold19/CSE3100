#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

typedef struct BoardTag {
   int row;
   int col;
   char** src;
} Board;

Board* makeBoard(int r,int c)
{
   Board* p = (Board*)malloc(sizeof(Board));
   p->row = r;
   p->col = c;
   p->src = (char**)malloc(sizeof(char*)*r);
   int i;
   for(i=0;i<r;i++)
      p->src[i] = (char*)malloc(sizeof(char)*c);
   return p;
}
void freeBoard(Board* b)
{
   int i;
   for(i=0;i<b->row;i++)
      free(b->src[i]);
   free(b->src);
   free(b);
}

Board* readBoard(char* fName)
{
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

void saveBoard(Board* b,FILE* fd)
{
   int i,j;
   for(i=0;i<b->row;i++) {
      fprintf(fd,"|");
      for(j=0;j < b->col;j++) 
         fprintf(fd,"%c",b->src[i][j] ? '*' : ' ');
      fprintf(fd,"|\n");
   }
}
void clearScreen()
{
   static const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
   static int l = 10;
   write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, l);
}

void printBoard(Board* b)
{
   clearScreen();	
   saveBoard(b,stdout);
   printf("------------------------------------------------------------\n");
}

int liveNeighbors(int i,int j,Board* b)
{
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

void evolveBoard(Board* src,Board* out)
{
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

typedef struct Task {
   int id;
   int from,to;
   int dirty;
   int inQ;
   Board *src;
   Board *dst;
} Task;

typedef struct SharedWork {
   Board* _b[2];
   int    iteration;
   int    nbTasks;
   int    nbInDone;
   int    pending;
   Task*  allTasks;
   Task** todo;
   Task** done;
   int    enter,exit;
   int    tInQ;
   pthread_mutex_t lock;
   pthread_cond_t spaceAvail;
   pthread_cond_t workAvail;
   pthread_cond_t startIteration;
} SharedWork;

SharedWork* makeSharedWork(char* fileName,int nbt)
{
   SharedWork* sw = malloc(sizeof(SharedWork));
   sw->_b[0] = readBoard(fileName);
   sw->_b[1] = makeBoard(sw->_b[0]->row,sw->_b[0]->col);
   sw->nbTasks  = nbt;
   sw->nbInDone = 0;
   sw->allTasks = (Task*)malloc(sizeof(Task)*nbt);
   sw->todo = (Task**)malloc(sizeof(Task*)*nbt);
   sw->done = (Task**)malloc(sizeof(Task*)*nbt);
   sw->enter = sw->exit = 0;
   sw->pending  = 0;
   sw->iteration = -1;
   pthread_mutex_init(&sw->lock,NULL);
   pthread_cond_init(&sw->startIteration,NULL);
   pthread_cond_init(&sw->spaceAvail,NULL);
   pthread_cond_init(&sw->workAvail,NULL);
   sw->tInQ = 0;
   int nbr = sw->_b[0]->row / nbt;
   int rem = sw->_b[0]->row % nbt;
   int from = 0;
   for(int k=0;k<nbt;k++) {
      int to = from + nbr + (rem>0 ? 1 : 0);
      rem -= 1;
      sw->allTasks[k] = (Task) {k,from,to,1,0,NULL,NULL};
      from = to;
   }
   return sw;
}
void freeSharedWork(SharedWork* sw)
{
   freeBoard(sw->_b[0]);
   freeBoard(sw->_b[1]);
   free(sw->todo);
   free(sw->done);
   free(sw->allTasks);
   pthread_mutex_destroy(&sw->lock);
   pthread_cond_destroy(&sw->spaceAvail);
   pthread_cond_destroy(&sw->workAvail);
   pthread_cond_destroy(&sw->startIteration);
}
void produceTask(SharedWork* sw,Task* theTask)
{
   pthread_mutex_lock(&sw->lock);
   while (sw->nbTasks - sw->tInQ  == 0)
      pthread_cond_wait(&sw->spaceAvail,&sw->lock);
   assert(sw->nbTasks - sw->tInQ > 0);
   sw->todo[sw->enter] = theTask;
   sw->enter = (sw->enter + 1) % sw->nbTasks;
   sw->tInQ += 1;
   sw->pending += 1;
   pthread_cond_signal(&sw->workAvail);
   pthread_mutex_unlock(&sw->lock);
}
Task* consumeTask(SharedWork* sw)
{
   pthread_mutex_lock(&sw->lock);
   while (sw->tInQ == 0) 
      pthread_cond_wait(&sw->workAvail,&sw->lock);
   Task* pickUp = sw->todo[sw->exit];
   sw->exit = (sw->exit + 1) % sw->nbTasks; 
   sw->tInQ -= 1;
   pthread_cond_signal(&sw->spaceAvail);
   pthread_mutex_unlock(&sw->lock);
   return pickUp;
}
void completeTask(SharedWork* sw,Task* t)
{
   pthread_mutex_lock(&sw->lock);
   sw->pending -= 1;
   if (sw->pending == 0)
      pthread_cond_signal(&sw->startIteration);
   pthread_mutex_unlock(&sw->lock);
}
void waitEndOfRound(SharedWork* sw)
{
   pthread_mutex_lock(&sw->lock);
   while (sw->pending > 0)
      pthread_cond_wait(&sw->startIteration,&sw->lock);
   pthread_mutex_unlock(&sw->lock);
}
void scheduleTask(SharedWork* sw,Task* t)
{
   pthread_mutex_lock(&sw->lock);
   //printf("task: %d < %d , %d >\n",t->id,t->dirty,t->inQ);
   if (t->dirty) {
      int n = (t->id + 1) % sw->nbTasks;
      int p = (t->id - 1 >= 0) ? t->id - 1 :  sw->nbTasks - 1;
      if (!t->inQ) {
         assert(sw->nbInDone < sw->nbTasks);
         sw->done[sw->nbInDone++] = t;
         t->inQ  = 1;
      }
      if (!sw->allTasks[n].inQ) {
         assert(sw->nbInDone < sw->nbTasks);
         sw->done[sw->nbInDone++] = sw->allTasks + n;
         sw->allTasks[n].inQ = 1;
      }
      if (!sw->allTasks[p].inQ) {
         assert(sw->nbInDone < sw->nbTasks);
         sw->done[sw->nbInDone++] = sw->allTasks + p;
         sw->allTasks[p].inQ = 1;
      }
      /*
        printf("schedule[%d]: %d[%d] <-  %d[%d] -> %d[%d]\n",sw->nbInDone,p,sw->allTasks[p].inQ,
             t->id,t->inQ,
             n,sw->allTasks[n].inQ);
      */
      assert(sw->nbInDone <= sw->nbTasks);
   }
   pthread_mutex_unlock(&sw->lock);
}
void dispatchDoneTask(SharedWork* sw,Board* s,Board* d)
{
   pthread_mutex_lock(&sw->lock);
   assert(sw->tInQ == 0);
   assert(sw->nbInDone <= sw->nbTasks);
   for(int i=0;i<sw->nbInDone;i++) {
      Task* t = sw->done[i];
      //printf("\tDispatched task[%d] [ %d,%d )\n",t->id,t->from,t->to);
      t->src = s;
      t->dst = d;
      t->dirty = t->inQ = 0;
      sw->todo[sw->enter] = t;
      sw->enter = (sw->enter + 1) % sw->nbTasks;
      sw->tInQ += 1;
      sw->pending += 1;
   }
   sw->nbInDone = 0;
   memset(sw->done,0,sizeof(Task*)*sw->nbTasks);
   pthread_cond_broadcast(&sw->workAvail);
   pthread_mutex_unlock(&sw->lock);
} 
void* evolveWithTask(SharedWork* sw)
{
   Task* t = consumeTask(sw);
   while (t) {
      //printf("thread %p task %p\n",pthread_self(),t);
      static int rule[2][9] = {
         {0,0,0,1,0,0,0,0,0},
         {0,0,1,1,0,0,0,0,0}
      };
      Board* src = t->src, *dst = t->dst;
      int dirty = 0;
      for(int i=t->from;i<t->to;i++) {
         for(int j=0;j< src->col;j++) {
            int ln = liveNeighbors(i,j,src);
            int c  = src->src[i][j];
            dirty = dirty || (c != rule[c][ln]);
            dst->src[i][j] = rule[c][ln];
         }
      }
      t->dirty = dirty;
      if (dirty)
         scheduleTask(sw,t);
      completeTask(sw,t);
      t = consumeTask(sw);
   }
   return NULL;
}


int main(int argc,char* argv[])
{
   int nbt=2;
   int nbi = atoi(argv[2]);
   SharedWork* sWork = makeSharedWork(argv[1],nbt);
   pthread_t wId[nbt];
   for(int i=0;i<nbt;i++) {
      pthread_create(wId,NULL,(void*(*)(void*))evolveWithTask,sWork);
   }
   printBoard(sWork->_b[0]);
   for(int i=0;i<nbt;i++) {
      Task* t = sWork->allTasks + i;
      t->src = sWork->_b[0];
      t->dst = sWork->_b[1];
      produceTask(sWork,t);
   }
   Board *s,*d;
   for(int g=1;g < nbi;g++) {
      waitEndOfRound(sWork);                  // this makes us wait until all produced tasks are done
      s = g & 0x1 ? sWork->_b[1] : sWork->_b[0];
      d = g & 0x1 ? sWork->_b[0] : sWork->_b[1];
      //printBoard(s);     
      //printf("iteration: %d\n",g);
      dispatchDoneTask(sWork,s,d);               // this takes the scheduled tasks and produces them again. 
      //usleep(100000);     
   }
   void* rv = NULL;
   for(int i=0;i<nbt;i++)
      produceTask(sWork,NULL);
   for(int i=0;i<nbt;i++) 
      pthread_join(wId[i],&rv);
   
   printBoard(d);
   FILE* final = fopen("final.txt","w");
   saveBoard(d,final);
   fclose(final);
   freeSharedWork(sWork);
}
