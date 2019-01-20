#ifndef __HISTOGRAM_H
#define __HISTOGRAM_H

#include <semaphore.h>

/****************
 * Bryan Arnold *
 *  Homework 7  *
 *   CSE 3100   *
 *  11/15/2017  *
 ***************/

//this is the Histogram struct, holds the data of the histogram,
//the name of the histogram/memory map zone, the semaphore's name,
//the file description area of the used memory zone, and a pointer
//to a smeaphore for managing access to the histogram
typedef struct Histogram {
  long*   _store;
  char    _zoneName[128];
  char    _semName[128];
  int     _zd;
  sem_t*  _lock;
} Histogram;

//Protoype functions
Histogram makeSharedHistogram(char* name);
void destroyHistogram(Histogram* h);

#endif
