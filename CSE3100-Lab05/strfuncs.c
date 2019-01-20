#include <stdio.h>
#include <stdlib.h>

/** Appends the src string to the end of the
 *  dest string. Return the resulting string. */
char* strcat_ptr(char* dest, char* src) {
	
	int i, j, sizeDest = 0, sizeSrc = 0;

	if(dest == NULL || src == NULL){

		return NULL;

	}

	for(i = 0; dest[i] != '\0'; i++){

		sizeDest++;

	}

	for(i = 0; src[i] != '\0'; i++){

		sizeSrc++;

	}

	char* catString = malloc(sizeof(char)*(sizeDest + sizeSrc + 1));

	for(i = 0; i < sizeDest; i++){

		catString[i] = dest[i];
		
	}

	for(j = sizeDest; j < (sizeDest + sizeSrc); j++){

		catString[j] = src[j - i];
		
	}

	catString[j] = '\0';

	return catString;

}

char* strcat_arr(char dest[], char src[]) {
	
	int sizeDest = 0, sizeSrc = 0, i, j;

	while(dest[sizeDest] != '\0'){

		sizeDest++;

	}

	while(src[sizeSrc] != '\0'){

		sizeSrc++;

	}

	char catString[sizeDest + sizeSrc + 1];

	for(i = 0; i < sizeDest; i++){

		catString[i] = dest[i];
		
	}

	for(j = sizeDest; j < (sizeDest + sizeSrc); j++){

		catString[j] = src[j - i];
		
	}

	catString[j] = '\0';

	char *appended = catString;
	
	return appended;
}


/** Searches the haystack string for the needle 
 *  substring. Return a pointer to the located 
 *  needle substring in the haystack string if 
 *  it exists (and the first if there are more 
 *  than one), or NULL if the needle is not in 
 *  the haystack. */
char* strstr_ptr(char* haystack, char* needle) {
	// TODO implement me using pointer syntax
	return NULL;
}

char* strstr_arr(char haystack[], char needle[]) {

	int sizeHay = 0, sizeNeed = 0, i, j, current, checker;
	char* found;

	while(haystack[sizeHay] != '\0'){

		printf("%c", haystack[sizeHay]);
		sizeHay++;

	}

	printf("\n");

	while(needle[sizeNeed] != '\0'){

		printf("%c", needle[sizeNeed]);
		sizeNeed++;

	}

	printf("\n");

	for(i = 0; i < sizeHay; i++){

		printf("Current haystack char: %c\n", haystack[i]);		
		current = i;		
		checker = sizeNeed - 1;

		if(haystack[i] == needle[0]){

			for(j = 1; j < sizeNeed + 1; j++){

				printf("Current needle char: %c\n", needle[j]);
				printf("Haystack char to compare: %c\n", haystack[j + current]);
				
				if(checker == 0){

					found = &haystack[i];
					printf("Found: %s\n", found);
					return found;

				} 
				
				if(haystack[current + j] != needle[j]){

					printf("%c and %c are not equal\n", haystack[j + i], needle[j]);
					break;

				} else{

					checker--;
					printf("Comparisons remaining for match: %d\n", checker);

				}

			}

		}

	}

	return NULL;
	
}


/** Searches for the first occurrence of the 
 *  character c in the string s and returns a
 *  pointer to it. If c does not appear in s,
 *  return NULL. */
char* strchr_ptr(char *s, char c) {

	return NULL;
}

char* strchr_arr(char s[], char c) {
	
	return NULL;
}

/** Returns a pointer to a new string which is
 *  a copy of the given string s. */
char* strdup_ptr(char* s) {
	
	if(s == NULL){

		return NULL;

	}

	int i, j, sizeStr = 0;

	for(i = 0; s[i] != '\0'; i++){

		sizeStr++;

	}

	char* newS = malloc(sizeof(char)*(sizeStr + 1));

	for(j = 0; j < sizeStr; j++){

		newS[j] = s[j];

	}

	newS[j] = '\0';

	return newS;

}

char* strdup_arr(char s[]) {
	
	int i = 0, j = 0;

	while(s[i] != '\0'){

		i++;

	}

	char newS[i + 1];

	for(j = 0; j < i; j++){

		newS[j] = s[j];

	}

	newS[j] = '\0';

	char* new = newS;

	return new;
}


/** Returns 1 if the strings s1 and s2 are the
 *  same, returns 0 otherwise. */
int streq_ptr(char* s1, char* s2) {
	// TODO implement streq using pointer syntax!
	return 0;
}

int streq_arr(char s1[], char s2[]) {
	// TODO implement streq using array syntax!
	return 0;
}


/** Main function. Add code to free allocated memory! 
 *  Valgrind should NOT yield any errors once you're done!
 *  DO NOT CHANGE OUTPUTS! JUST ADD CLEAN UP CODE!  */
int main(int argc, char** argv) {
	
	/* Read strings from program arguments */
	if(argc != 3) {
		printf("usage: ./strfuncs s1 s2\n");
		return 1;
	}
	char* s1 = argv[1];
	char* s2 = argv[2];
	printf("String 1: %s\n", s1);
	printf("String 2: %s\n\n", s2);
	
	/* Check for string equality */
	int s1eqs2ptr = streq_ptr(s1, s2);
	int s1eqs2arr = streq_arr(s1, s2);
	printf("ptr: s1=s2? %s\n", s1eqs2ptr ? "yes" : "no");
	printf("arr: s1=s2? %s\n\n", s1eqs2arr ? "yes" : "no");
	
	/* Concatenate s1 to s2 and s2 to s1 */
	char* s1s2ptr = strcat_ptr(s1, s2);
	printf("ptr: s1+s2=%s\n", s1s2ptr);
	free(s1s2ptr);
	char* s2s1ptr = strcat_ptr(s2, s1);
	printf("ptr: s2+s1=%s\n", s2s1ptr);
	free(s2s1ptr);
	char* s1s2arr = strcat_arr(s1, s2);
	printf("arr: s1+s2=%s\n", s1s2arr);
	char* s2s1arr = strcat_arr(s2, s1);
	printf("arr: s2+s1=%s\n\n", s2s1arr);

	/* Check for substrings */
	char* s1ins2ptr = strstr_ptr(s2, s1);
	char* s2ins1ptr = strstr_ptr(s1, s2);
	char* s1ins2arr = strstr_arr(s2, s1);
	char* s2ins1arr = strstr_arr(s1, s2);
	printf("ptr: s1 in s2 -> %s\n", s1ins2ptr == NULL ? "no" : s1ins2ptr);
	printf("ptr: s2 in s2 -> %s\n", s2ins1ptr == NULL ? "no" : s2ins1ptr);
	printf("arr: s1 in s2 -> %s\n", s1ins2arr == NULL ? "no" : s1ins2arr);
	printf("arr: s2 in s1 -> %s\n\n", s2ins1arr == NULL ? "no" : s2ins1arr);
	
	/* Check for character occurence */
	char* ains1ptr = strchr_ptr(s1, 'a');
	printf("ptr: 'a' in s1? %s\n", ains1ptr == NULL ? "no" : ains1ptr);
	free(ains1ptr);
	char* ains1arr = strchr_arr(s1, 'a');
	printf("arr: 'a' in s1? %s\n\n", ains1arr == NULL ? "no" : ains1arr);
	
	/* Check duplication of strings */
	char* dups1ptr = strdup_ptr(s1);
	printf("ptr: dup(s1)=%s\n", dups1ptr);
	free(dups1ptr);
	char* dups1arr = strdup_arr(s1);
	printf("arr: dup(s1)=%s\n", dups1arr);
	
	/* Clean up, i.e. free memory! */
	// TODO implement code to clean up!
	
	/* Done! */
	return 0;
}

