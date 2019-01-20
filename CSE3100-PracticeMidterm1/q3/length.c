#include <stdio.h>
#include <stdlib.h>

int length(char* s)
{

	if(s == NULL){

		return 0;

	}
 
	int size = 0;

	while(s[size] != '\0'){

		size++;

	}

	return size;

}

/*
 * Do not modify the main function!
 */
int main()
{
   char buf[512];
   for(int i=0;i<sizeof(buf);i++)
     buf[i] = 0;
   scanf("%511[^\n]",buf);
   int sl = length(buf);
   printf("length = %d\n",sl);
   return 0;
}
