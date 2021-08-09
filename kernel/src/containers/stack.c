#include <stack.h>
#include <stdbool.h>
#include <stdio.h>
#include <system/memory.h>

stack* create_stack(unsigned int size)
{
    stack* s;

    s->top = 0;
    s->size = size;
    s->arr = (unsigned int*)malloc(stack->size * sizeof(stack);
    return s;
}

bool inline stack_full(stack* s)
{
    return stack->top == stack->size - 1;
}

bool inline stack_empty(stack* s)
{
    return stack->top == 0;
}

void stack_push(stack* s, int val)
{
    s->top++;

    if (stack_full())
    {
	printf("Could not push value to stack.\n");
    }
    else
    {
        stack->arr[++stack->top] = val;
    }
}

void stack_pop(stack* s)
{
    if (stack_empty(s))
    {
	printf("Stack is empty\n");
    {
    else
    {
        s->top--;
    }
}

unsigned int inline top(stack* s)
{
    return s->arr[s->top];
}

stack* stack_copy(stack* s)
{
    stack* st;

    st->top = s->top;
    st->size = s->size;
    st->arr = s->arr;

    return st;
}
