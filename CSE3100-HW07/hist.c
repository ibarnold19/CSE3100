#include "hist.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

/****************
 * Bryan Arnold *
 *  Homework 7  *
 *   CSE 3100   *
 *  11/15/2017  *
 ***************/


//makeSharedHistogram Function
//This function utilizes the histogram struct in hist.h
//to create a histogram. the data of the histogram is
//a shared memory map and the other values of hist.h are
//the name of the semaphore, the memory map, as well as the file
//descriptive file zone created below.
Histogram makeSharedHistogram(char* name){

	//Make new hist struct
	struct Histogram hist;

	//Create the zone name which si the histogram name
	const char* zone = name;
	int pshared = 1;
	unsigned int value = 1;

	//Create the shared memory map
	int md = shm_open(zone, O_RDWR|O_CREAT, S_IRWXU);
	ftruncate(md, 4096);
	long* ptr = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, md, 0);

	//Create a semaphore for managing processes access to the data in histogram
	sem_t* s = sem_open("/sema2", O_CREAT, 0666, 1);
	sem_init(s, pshared, value);
	memset(ptr, 0, 4096);
	
	//Set the struct's members to values just created
	hist._store = ptr;
	strcpy(hist._zoneName, name);
	strcpy(hist._semName, "/sema2");
	hist._zd = md;
	hist._lock = s;

	return hist;	

}

//destroyHistogram Function
//This function simply gets rid of all the
//resources used by the histogram creation
//in the previous function
void destroyHistogram(Histogram* h){
  
	munmap(h->_store, 4096);
	close(h->_zd);
	shm_unlink(h->_zoneName);
	sem_close(h->_lock);
	sem_unlink(h->_semName);


}
