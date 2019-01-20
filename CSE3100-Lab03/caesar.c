#include <stdio.h>
#include <stdlib.h>

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *    Lab 3     *
 *   9/23/17    *
 ***************/


/*
 * obtains the filesize by "seeking" at the end and restoring the seek pointer
 * to where it used to be before the call to filesize. 
 */
long fileSize(FILE* f){

	long position = ftell(f);
	fseek(f, 0, SEEK_END);
	long length = ftell(f);
	rewind(f);

	return length;

}

void caesar(long sz,char buf[sz],int shift);

/*
 * read the file into an array and use the caesar encoder with the specified shift
 * - f     : the file to read from
 * - shift : the shift in the cipher [http://practicalcryptography.com/ciphers/caesar-cipher/]
 * Hint: use fileSize to figure out how much space you need. 
 *       Caesar never sent very long messages to his generals ;-)
 */
void readAndEncode(FILE* f,int shift){

	long size = fileSize(f);
	char text[size];

	while(fgets(text, size, f)){

		fscanf(f, "%s", text);

	}

	
	caesar(size, text, shift);

}

/*
 * The actual caesar encoder. Input is:
 * - sz    : the buffer size
 * - buf   : actual buffer
 * - shift : how much to shift by
 */
void caesar(long sz,char buf[sz],int shift){
	
	for(int i = 0; i < sz; i++){
		
		/* If current char isn't a space */
		if(buf[i] != ' '){ 

			buf[i] = buf[i] + shift;
			
			/* If the shift goes over 'z' value, go back to a */
			if(buf[i] > 122){
	
				buf[i] = buf[i] - 26;
				printf("%c", buf[i]);

			} else {

			printf("%c", buf[i]);

			}

		} else {

			printf(" ");

		}

	}

	printf("\n");


}

/*
 * main program. Input on command line (2):
 * - first the shift
 * - second the filename of the secret to be encoded
 * Do error checking (we need 2 inputs)
 * The shift should be in (0..26)
 * We should have a filename
 * Print the encoded message on the standard output
 */
int main(int argc,char* argv[])
{
  if (argc != 3) {
    printf("usage: caesar <shift> <filename>\n");
    return 1;
  }
  int shift = atoi(argv[1]);
  char* fName = argv[2];
  if (shift < 0 || shift > 26) {
    printf("caesar shift should be a value in [0..26]\n");
    return 1;
  }
  FILE* src = fopen(fName,"r");
  readAndEncode(src,shift);
  fclose(src);
  return 0;
}
