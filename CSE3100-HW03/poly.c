#include <stdio.h>
#include <stdlib.h>

/****************
 * Bryan Arnold *
 *  Homework 3  *
 *   CSE 3100   *
 *   10/4/17    *
 ***************/

/*
 * Polynomial ADT. This data type uses two structures
 * Monomial: is used to represent a term  c * x^p
 *           where c is the coefficient and p is the power.
 * Polynomial: is used to capture an entire polynomial as a list
 *             of monomial. 
 * Note that the representation is sparse. Monomials with a coefficient
 * of zero should not be in the list. For instance, the polynomial:
 * 3 + 2 x + 4 x^3 - 7 x^10 would be represented by:
 * (10 , [(3,0), (2,1),(4,3),(-7,10)])
 * Where the first 10 is the degree of the polynomial and the list 
 * represent the 4 monomials with non-zero coefficients. 
 */
typedef struct Monomial {
    int coeff;     /* coefficient */
    int exp;       /* exponent    */
    struct Monomial *next;  /* next monomial */
} Mono;

typedef struct Polynomial {
    int   deg;     /* degree */
    Mono* first;   /* first monomial */
    Mono* last;    /* last monomial  */
} Poly;

/*
 * This function creates a new monomial with a given coefficient and power.
 * c : the coefficient
 * k : the power
 * The function allocates a monomial and initializes its attributes.
 * The return value is a pointer to the newly created monomial.
 */
Mono* newMono( int c, int k) {
   
	Mono* mono = (Mono*)malloc(sizeof(Mono));
	if(mono == NULL) return NULL;
	
	mono->coeff = c;
	mono->exp = k;
	mono->next = NULL;

	return mono;

}

/*
 * This function creates a new (empty) polynomial with degree 0 and no
 * monomials. 
 */
Poly* newPoly() {
   
	Poly* poly = (Poly*)malloc(sizeof(Poly));
	if(poly == NULL) return NULL;

	poly->deg = 0;
	poly->first = NULL;
	poly->last = NULL;

	return poly;

}

/*
 * This function deallocates a polynomial.
 * p : the polynomial to deallocate
 * The function deallocates not-only the polynomials but also all the
 * monomials that it refers to (since those should not be shared anyway!)
 */
void freePoly(Poly* p) {
   
	if(p == NULL) return;

	Mono* m = p->first;

	while(m != NULL){

		Mono* m1 = m->next;
		free(m);
		m = m1;

	}

	free(p);

}

/*
 * This functions adds a monomial inside a polynomial
 * p : the polynomial to modify
 * m : the monomial to add
 * The polynomial p is expected to be the sole owner of m. 
 * The new monomial should be added at the end of the list.
 * The degree of the monomial should not exceed the degree of the polynomial
 */
void appendMono( Poly* p, Mono* m) {
   
	if(p->first == NULL){

		p->first = p->last = m;

	} else {

		p->last = p->last->next = m;

	}

}

/*
 * This function allocates, reads and returns a polynomial.
 * It starts by asking the highest degree
 * Then it reads all the monomials (which are given in increasing 
 * order of powers) as pairs of integers (whitespace separated) 
 * and adds them to the polynomial.
 * It finally returns the constructed poly.
 */
Poly* readPoly() {

	int c, k;
  
	Poly* p = newPoly();

	if(p == NULL) return NULL;

	scanf("%d", &p->deg);

	do{

		scanf("%d %d", &c, &k);
		appendMono(p, newMono(c, k));

	} while(k < p->deg);

	return p;

}

/*
 * This function prints the received polynomial on the standard output.
 * p  : the polynomial to print.
 * Ouput: print the degree, then print a sequence of pairs coef power
 * end with a line feed. 
 */
void printPoly(Poly* p ) {
   
	if(p == NULL || p->first == NULL){

		printf("empty\n");
		return;

	} else {

		printf("%d ", p->deg);
		Mono* m = p->first;
		while(m != NULL){

			printf("%d %d ", m->coeff, m->exp);
			m = m->next;

		}

		printf("\n");

	}

}

/*
 * The addPoly function computes a new polynomial that represent the sum of
 * its inputs. 
 * p1  : first polynomial
 * p2  : second polynomial
 * Assumptions: 
 * - both p1 and p2 list monomials in increasing powers.
 * - p1 and p2 do NOT have to have the same degree
 * - p1 and p2 are NOT expected to have the same number of monomials
 * - p1 and p2 are NOT expected to have monomials of matching degrees (they might)
 * - The treatment is NOT destructive. p1 and p2 are untouched.
 * Output:
 * A new polynomial is allocated, filled and returned. 
 * Notes:  
 * - monomials of the same power from p1 and p2, should be aggregated.
 * - monomials with a zero coefficient should be discarded.
 * - the degree of the answer should be the highest power with a non-zero coef. 
 */
Poly* addPoly(Poly* p1,Poly* p2) {
   
	Poly* result = newPoly();
	Mono* m1 = (p1 == NULL ? NULL : p1->first);
	Mono* m2 = (p2 == NULL ? NULL : p2->first);

	while(m1 && m2){

		if(m1->exp == m2->exp){

			if(m1->coeff + m2->coeff != 0){

				appendMono(result, newMono(m1->coeff + m2->coeff, m1->exp));

			}

			m1 = m1->next;
			m2 = m2->next;

		} else if(m1->exp < m2->exp){
	
			appendMono(result, newMono(m1->coeff, m1->exp));
			m1 = m1->next;

		} else {

			appendMono(result, newMono(m2->coeff, m2->exp));
			m2 = m2->next;		

		}

	}

	while(m1){

		appendMono(result, newMono(m1->coeff, m1->exp));
		m1 = m1->next;

	}

	while(m1){

		appendMono(result, newMono(m2->coeff, m2->coeff));
		m2 = m2->next;

	}

	if(result->last != NULL){

		result->deg = result->last->exp;

	} else {

		result->deg = 0;
		return result;

	}

}

/*
 * The test program is provided!
 */

int main() {
    Poly* p1 = readPoly();
    Poly* p2 = readPoly();
    Poly* sum;

    if( (p1 == NULL) || (p2 == NULL) )
    {
        fprintf(stderr, "Could not allocate memory\n");
        return 1;
    }
    printPoly(p1);
    printPoly(p2);
    printf("\n");
    
    printPoly( sum = addPoly( p1, p2 ) );

    freePoly(p1);
    freePoly(p2);
    freePoly(sum);
    return 0;
}

/***************************************************/
