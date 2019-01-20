/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *   9/8/2017   *
 *    Lab 1     *
 ***************/ 

#include <stdio.h>

int main(){

	int n, sum;

	printf("n:");
	scanf("%d", &n);


	if(n >= 0){

	int i = n;

		while(i <= 2 * n){

			sum = sum + i;
			i++;

		}

	} else {

	int j = 2 * n;
   
		while(j <= n){

			sum = sum + j;
			j++;

		}

	}

	printf("sum: %d \n", sum);
	return 0;

}
