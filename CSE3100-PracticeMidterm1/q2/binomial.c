#include <stdio.h>

int binomial(int n,int k)
{

	if(k > n) {

		printf("n must be greater than or equal to k\n");
		return 0;

	}
  
	if(k == 0){

		 return 1;

	} else if(k > 0){

		return (n / k) * binomial(n - 1, k - 1);	

	}

}

/*
 * Do NOT modify the main function!
 */
int main()
{
  int n,k;
  int nbRead = scanf("%d %d",&n,&k);
  if (nbRead != 2) return 1;
  printf("binomial (%d %d) = %d\n",n,k,binomial(n,k));
  return 0;
}
