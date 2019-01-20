/****************
 * Bryan Arnold *
 *   CSE 3100   *
 *   9/8/2017   *
 *    Lab 1     *
 ***************/

#include <stdio.h>


int main(){

	long a, b, exponent;

	long long product = 1;

	printf("a: ");
	scanf("%ld", &a);
	printf("b: ");
	scanf("%ld", &b);

	exponent = b;

	while(exponent != 0){

		product *= a;
		--exponent;

	}

	printf("(%ld)^(%ld)=%lld\n", a, b, product);
	return 0;

}
