#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__




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

typedef struct SharedWork {
   Board* _b[2];
   int    iteration;
   int   nbWorkers;
   int   done;
   pthread_barrier_t iterBar;
} SharedWork;

typedef struct Worker {
   SharedWork* sWork;
   pthread_t   id;
   int   nbIterations;
   int   fromRow;
   int   toRow;
} Worker;

SharedWork* makeSharedWork(char* fileName,int nbw)
{
   SharedWork* sw = malloc(sizeof(SharedWork));
   sw->_b[0] = readBoard(fileName);
   sw->_b[1] = makeBoard(sw->_b[0]->row,sw->_b[0]->col);
   sw->iteration = -1;
   sw->nbWorkers = nbw;
   sw->done = nbw;
   pthread_barrier_init(&sw->iterBar,NULL,nbw + 1);
   return sw;
}
void freeSharedWork(SharedWork* sw)
{
   freeBoard(sw->_b[0]);
   freeBoard(sw->_b[1]);
   pthread_barrier_destroy(&sw->iterBar);
}
void evolveOnce(SharedWork* sw)
{
   pthread_barrier_wait(&sw->iterBar);
   sw->iteration += 1;
}

void* evolveWithWorker(Worker* w)
{
   Board* b0,*b1;
   for(int g=0;g< w->nbIterations;g++) {
      static int rule[2][9] = {
         {0,0,0,1,0,0,0,0,0},
         {0,0,1,1,0,0,0,0,0}
      };
      b0 = (g & 0x1) ? w->sWork->_b[1] : w->sWork->_b[0];
      b1 = (g & 0x1) ? w->sWork->_b[0] : w->sWork->_b[1];

      for(int i=w->fromRow;i<w->toRow;i++) {
         for(int j=0;j< b0->col;j++) {
            int ln = liveNeighbors(i,j,b0);
            int c  = b0->src[i][j];
            b1->src[i][j] = rule[c][ln];
         }
      }
      pthread_barrier_wait(&w->sWork->iterBar);
   }
   return NULL;
}


int main(int argc,char* argv[])
{
   int nbw = 2;
   int nbi = atoi(argv[2]);
   SharedWork* sWork = makeSharedWork(argv[1],nbw);
   Worker wArray[nbw];
   int from = 0, nbl = sWork->_b[0]->row / nbw,rem = sWork->_b[0]->row % nbw;
   for(int i=0;i<nbw;i++) {
      wArray[i].sWork = sWork;
      wArray[i].nbIterations = nbi;
      wArray[i].fromRow = from;
      wArray[i].toRow   = from = from + nbl + (rem ? 1 : 0);
      rem = rem ? rem - 1 : 0;
      pthread_create(&wArray[i].id,NULL,(void*(*)(void*))evolveWithWorker,wArray+i);
   }
   int g;
   for(g=0;g < nbi;g++) {
      //printBoard((g & 0x1) ? sWork->_b[1] : sWork->_b[0]);
      //printf("iteration: %d\n",g);
      evolveOnce(sWork);
      //usleep(100000);
   }
   printf("before join...\n");
   for(int i=0;i<nbw;i++) {
      pthread_join(wArray[i].id,NULL);
      printf("join: %d --- %p\n",i,wArray[i].id);
   }
   printf("after join...\n");
   Board* finalBoard = g & 0x1 ? sWork->_b[1] : sWork->_b[0];
   printBoard(finalBoard);
   FILE* final = fopen("final.txt","w");
   saveBoard(finalBoard,final);
   fclose(final);
   freeSharedWork(sWork);
}
