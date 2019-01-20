#include <stdio.h>

/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *     Lab 3    *
 *    9/23/17   *
 ***************/

/* Prototype declaration of the toUpper function */
void toUpper(char s1[]);

/* Prototype declaration of the toLower function */
void toLower(char s1[]);

/**********************************
 * toUpper function implementation.
 * This function takes "Hello World!'
 * as input and returns each character
 * as it's capitalized version.
 **********************************/

void toUpper(char s1[]){

	/* Hello world! is 13 chars long */
	for(int i = 0; i < 13; i++){

		/* IF uppercase do noting, else make uppercase */
		if(s1[i] < 97 || s1[i] > 122){

		} else {

			s1[i] = s1[i] - 32;

		}

	}


}

/*********************************
 * toLower function implementation.
 * This function takes Hello World!
 * and turns each character into it's
 * lowercase form.
 *********************************/

void toLower(char s1[]){

	/* Heelo world! is only 13 chars long */
	for(int i = 0; i < 13; i++){

		/* If already lowercase do nothing, else make uppercase */
		if(s1[i] < 65 || s1[i] > 90){

		} else {

			s1[i] = s1[i] + 32;			

		}

	}


}


int main(){

   char s1[] = "Hello World!";

   toUpper(s1);
   printf("result: %s\n",s1);

   toLower(s1);
   printf("result: %s\n",s1);

   return 0;


}
