#ifndef __STACK__
#define __STACK__

#include <stdbool.h>

typedef struct stack
{
    unsigned int top;
    unsigned int size;
    unsigned int* arr;
} stack;

stack* stack_create(unsigned int size);
bool stack_full(stack* s);
bool stack_empty(stack* s);
unsigned int inline top(stack* s);
void stack_push(stack* s, int val);
void stack_pop(stack* s);
unsigned int inline top(stack* s);
stack* stack_copy(stack* s);

#endif
