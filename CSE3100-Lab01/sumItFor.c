/******************
** Bryan Arnold ***
**   CSE 3100   ***  
**   9/8/2017   ***
**    Lab 1     ***
******************/

#include <stdio.h>

int main(){

	int n, sum;

	printf("n:");
	scanf("%d", &n);

	if(n >= 0){

		for(int i = n; i <= 2 * n; i++){
		
			sum = sum + i;

		}		

	} else {

		for(int j = 2 * n; j <= n; j++){ 

			sum = sum + j;

		}	

	}

	printf("sum: %d  \n", sum);
	return 0;

}
