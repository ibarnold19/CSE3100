#include <stdio.h>
#include <stdlib.h>

typedef struct Stack {
   int* data;
   int  top;
   int  sz;
} Stack;

Stack* makeStack(int sz);
void freeStack(Stack* s);
void pushStack(Stack* s,int v);
int  popStack(Stack* s);
int  topStack(Stack* s);

/*
 * emptyStack is a convenience macro that tests whether a stack s is 
 * empty or not. 
 */
#define emptyStack(s) ((*s).top == 0)

int main() {
   int msz = 0,nbPush = 0;
   scanf("%d %d",&msz,&nbPush);

   Stack* s = makeStack(msz);
   for(int i=0;i<nbPush;i++) {
      pushStack(s,i);
      printf("pushed: %d\n",topStack(s));
   }
   while (!emptyStack(s)) {
      printf("on Stack: %3d\n", popStack(s));
   }
   printf("freeing\n");
   freeStack(s);
   return 0;
}

/*
 * All your ADT functions should be implemented below.
 * 
 */

/*
 * This function allocates a stack of maximal size sz on the heap.
 * The stack must start off empty
 * Input:
 * - sz : the maximal size the stack can have (until a resize must occur). 
 */
Stack* makeStack(int sz)
{
  
	Stack* stack = (Stack*)malloc(sizeof(Stack)*sz);

	stack->top = 0;
	stack->sz = sz;
	stack->data = (int*)malloc(sizeof(int)*sz);

	return stack;

}

/*
 * freeStack deallocates the input stack 's' (currently on the heap).
 * You cannot have any memory leaks
 * Input: 
 * - s : a pointer to the stack to deallocate
 */
void freeStack(Stack* s)
{	

	free(s->data);
	free(s);

}

/*
 * pushStack adds a new element on top of the stack. If the stack is full
 * it should automatically resize itself to accommodate the new element. 
 * Inputs:
 * - s a pointer to the stack to push an element on
 * - v the value to push at the top of the stack. 
 */
void pushStack(Stack* s,int v)
{
  
	int i = s->sz - 1;

	if(emptyStack(s)){

		s->data[0] = v;
		s->top = v;

	} else {

		while(i > 0){

			s->data[i] = s->data[i - 1];
			i--;

		}

	}

	s->data[0] = v;
	s->top = v; 


}
/*
 * popStack removes and returns the element at the top of the stack.
 * The function behaves correctly if and only if the stack is not empty.
 * If the stack is empty, the behavior is undefined. 
 * Input: 
 * - s : a pointer to the stack to query
 */
int  popStack(Stack* s)
{
  
	int i = 0;
	int result = s->top;

	if(emptyStack(s)){

		printf("Undefined behavior\n");
		return 0;

	} else {

		while(i < s->sz - 1){

			s->data[i] = s->data[i + 1];
			i++;
			
		}

	}

	s->data[i] = 0;
	s->top = s->data[0];

	return result;

}

/*
 * topStack returns a copy of the element at the top of the stack.
 * Input: 
 * - s : a pointer to the stack to query
 */
int  topStack(Stack* s)
{
 
	int top = s->top;


	return top;

}
