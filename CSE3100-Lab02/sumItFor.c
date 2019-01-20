#include <stdio.h>

int main() {
	int n,sum = 0,i;
	printf("n: ");
	scanf("%d", &n);

	if(n >= 0)
		for(i = n; i <= 2*n; i++)
			sum += i;
	else
		for(i = 2*n; i <= n; i++)
			sum += i;
	printf("sum: %d\n", sum);
	return 0;
}
