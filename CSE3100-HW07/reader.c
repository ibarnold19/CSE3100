#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include "hist.h"

/****************
 * Bryan Arnold *
 *  Homework 7  *
 *   CSE 3100   *
 *  11/15/2017  *
 ***************/

//Struct for the file map. Contains a pointer
//to the file in a shared memory map, the name of the file,
//the file descriptor of the file opened in shared memory,
//the file size in bytes, the maximum size of the memory zone
//in bytes.
typedef struct FileMap {
    unsigned char* ptr;
    char* fileName;
    int   md;
    long  fsz;
    long  msz;
} FileMap;

//Prototype for checkError
void checkError(int status);

//Just to just if something went wrong in certain functions
void checkError(int status){

	if(status < 0){

		printf("Error: [%s]\n", strerror(errno));
		exit(-1);

	}

} //end function

//mapFile Function
//This function creates a shared memory zone with the contents of a given
//file incase the file is too big.
FileMap mapFile(char* fName) {

	//Stat struct to get the size of the file and a new FileMap struct to be created
	struct stat fs;
	struct FileMap map;

	//Get size of the file and check if an error happened while counting
	int status = stat(fName, &fs);
	checkError(status); 

	//size of the file, the pages, and the file descriptor location when opened
	int size = fs.st_size;
	int pg = (size / 4096) + 1;
	int md = open(fName, O_RDWR);

	//Create the memory zone to be shared for reading and check if an error occured while doing this
	unsigned char* ptr = mmap(NULL, pg * 4096, PROT_READ|PROT_WRITE, MAP_SHARED, md, 0);
	if(ptr == MAP_FAILED){

		printf("mmap failed: [%s]\n", strerror(errno));
		exit(1);

	}

	//Set struct's member to appropriate values above and return
	map.ptr = ptr;
	map.fileName = fName;
	map.md = md;
	map.fsz = fs.st_size;
	map.msz =  4096 * ((map.fsz + 4096 - 1) / 4096);	

	return map;

}

//destroyMap Function
//This functions simply closes and deallocates any
//resources used when creating the fileMap previously
void destroyMap(FileMap fm) {
  
	int status = munmap(fm.ptr, fm.fsz);
	checkError(status);
	close(fm.md);
	shm_unlink(fm.fileName);
	fm.ptr = NULL;
	fm.fsz = 0;
	fm.msz = 0;

}


int main(int argc,char* argv[]) {

//Check for correct input
if (argc < 3) {
    
	printf("Usage is: histogram <filename> <#workers>\n");
	return 1;

}

//Assign values from command line for struct creation
char* filename = argv[1];
int workers = atoi(argv[2]);
char* histName = "histogram";
  
//Create a FileMap andhistogram for computations later on
FileMap map = mapFile(filename);
Histogram h = makeSharedHistogram(histName);

int i;
int n = workers;
long workload = map.fsz / workers;
long processed = 0;
long position = 0;
long pages = ((map.fsz % 4096) ? map.fsz / 4096 + 1 : map.fsz / 4096);
pid_t pids[workers];

//Start looping of n processes, or n workers
for(i = 1; i <= workers; ++i){

	//Error occured in forking, so report it and exit
	if((pids[i] = fork()) < 0) {

		printf("Forking error\n");
		abort();

	//Child process
	} else if(pids[i] == 0){
	
		//Only do amount of work assigned to each worker
		position = workload*(i-1);
		
		//Place into the histogram storage the current character's ASCII value, use semaphores to manage turns
		while(position != workload*i){

			sem_wait(h._lock);
			h._store[(int)map.ptr[position]]++;
			position++;
			sem_post(h._lock);

		}

		//Child done so exit
		exit(0);		

	} else {

		int status;
		pid_t pid;
	
		//Wait for the child process currently working to finish and then increment the amount of bytes processed
		pid = waitpid(pids[i], &status, 0);
		processed += workload;

	}

}

	//Clean up if all the bytes in the file weren't placed into the histogram, a just in case thing basically
	int j;
	if(processed != map.fsz){

		for(j = processed; j < map.fsz; ++j){

			h._store[(int)map.ptr[j]]++;

		}

	}

	printf("Mapping a zone with %ld pages (%ld bytes)\n", pages, map.msz);

	//Print out the histogram
	int k;
	for(k = 0; k < 256; ++k){

		printf("h[%d] = %d\n", k, h._store[k]);

	}

	//Cleanup of resources used by the histogram and FileMap
	Histogram* histo = &h;
	destroyHistogram(histo);
	destroyMap(map);

	return 0;

}
