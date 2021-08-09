#ifndef __STACK__
#define __STACK__

typedef struct stack
{
    unsigned int top;
    unsigned int size;
    int* arr;
} stack __attribute__((packed));

stack* stack_create(unsigned int size);
bool inline stack_full(stack* s);
bool inline stack_empty(stack* s);
void stack_push(stack* s, int val);
void stack_pop(stack* s);
unsigned int inline top(stack* s);
stack* stack_copy(stack* s);

#endif
