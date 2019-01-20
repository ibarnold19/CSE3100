#include <stdio.h>
#include <stdlib.h>

int fact(int n);

int main(int argc, char* argv[]) {
	
	if(argc < 2) {
		printf("usage is: ./fact n");
		return 1;
	}
	int n = atoi(argv[1]);
	int r = fact(n);
	printf("Factorial(%d) = %d\n", n, r);
	return 0;
}

int fact(int n) {
	if(n == 0)
		return 1;
	return n * fact(n-1);
}
