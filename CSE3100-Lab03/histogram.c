#include <stdio.h>
#include <stdlib.h>

#define HSZ 256

/*
 * Compute a histogram (frequency of characters) for the content of the text file f
 * It should build a frequency table, initialize it and fill it up. 
 * It should print the table entries that are non-zero at the end.
 */
void makeHistogram(FILE* f){

	long position = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);

	char text[size];

	while(fgets(text, size, f)){

		fscanf(f, "%s", text);

	}

	/* ASCII values for all printable characters */
	int counts[HSZ] = {0};
	
	/* Number of times each character is found */
	for(int i = 0; i < size; i++){

		counts[(int)(text[i])]++;

	}

	/* Print each character in ASCII and it's occurences if it occured */
	for(int j = 1; j < HSZ; j++){

		if(counts[j] != 0){

			printf("h[%d] = %d\n", j, counts[j]);

		}

	}
	
}

int main(int argc,char* argv[])
{
  if (argc != 2) {
    printf("usage: histogram <filename>\n");
    return 1;
  }
  char* fName = argv[1];
  FILE* src = fopen(fName,"r");
  makeHistogram(src);
  fclose(src);
  return 0;
}
