#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCKSIZE  4096

void make_copy(int i, char* string) {
    int len;
    char *s;
    
    len = strlen(string);
    s = (char*)malloc(sizeof(char)*len);
    strncpy(s, string, len);

    if(!(i%1000)) 
        printf("i=%d, %s\n", i, s);
	
    free(s);

}

int main() {
    int i;
    char *ptr, *string = "find the bugs!";

    for(i=0; i<10000; i++) {
        ptr = (char*)malloc(sizeof(char)*BLOCKSIZE);
        strcpy(ptr, string);
        make_copy(i, ptr);
	free(ptr);
    }

	return 0;

}

