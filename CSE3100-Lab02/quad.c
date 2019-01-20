#include <stdio.h>
#include <math.h>

double x_to_y(double x, int y);

double x_to_y(double x, int y) {
	if(y == 1)
		return x;
	return x * x_to_y(x, y-1);
}

int main() {
	double a, b, c;
	double b2;
	int is_imag;
	double D, z1, z2;
	
	printf("a: ");
	scanf("%lf", &a);
	printf("b: ");
	scanf("%lf", &b);
	printf("c: ");
	scanf("%lf", &c);
	
	b2 = x_to_y(b, 2);
	D = b2 - 4.0L*a*c;
	
	if(D < 0) {
		z1 = -b / (2*a);
		D = -D;
		D = sqrt(D);
		z2 = D / (2*a);
		is_imag = 1;
	}else{
	
	D = sqrt(D);
	z1 = (-b + D) / (2*a);
	z2 = (-b - D) / (2*a);
	
	}

	if(is_imag) {
		printf("x1 = %lf + %lfi\n", z1, z2);
		printf("x2 = %lf - %lfi\n", z1, z2);
	}else{
		printf("x1 = %lf\n", z1);
		printf("x2 = %lf\n", z2);
	}
	
	return 0;
}
