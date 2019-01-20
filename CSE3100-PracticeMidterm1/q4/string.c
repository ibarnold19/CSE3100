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

char* strrev(char* s)
{

	if(s == NULL){

		return NULL;

	}

	int i = 0;

	int size = length(s);

	char* newS = malloc(sizeof(char)*size);

	while(size > 0){

		newS[i] = s[size - 1];
		i++;
		size--;

	}

	newS[i] = '\0';	

	return newS;

}


/*
 * Do NOT modify the main function!
 */
int main()
{
   char buf[512];
   for(int i=0;i<sizeof(buf);i++)
     buf[i] = 0;
   scanf("%511[^\n]",buf);
   char* rb = strrev(buf);
   printf("reverse of [%s] = [%s]\n",buf,rb);
   free(rb);
   return 0;
}
