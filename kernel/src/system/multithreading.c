#include <system/multithreading.h>
#include <system/memory.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>
#include <algorithms/algorithms.h>
#include <system/sys_kern.h>
#include <containers/stack.h>

task_manager* create_task_manager()
{
    task_manager* tm = malloc(sizeof(task_manager));
    tm->num_tasks = 0;
    tm->current_task = 0xFF;
    return tm;
}

//CREATES A STACK SPECIFICALLY FOR A TASK
stack* create_task_stack()
{
    return stack_create(4096);
}

task* task_create(unsigned int pid, unsigned int priority, void (*function_ptr)())
{
    task* t = malloc(sizeof(task));

    t->curr_ins = function_ptr;
    t->pid = pid;
    t->priority = priority;
    t->state = NEW;
    t->task_stack = create_task_stack();

    return t;
}

//CHECKS IF THE TASK MANAGER'S TASK ARRAY IS FULL
bool manager_full(task_manager* manager)
{
    return manager->num_tasks == 256;
}

//CHECKS IF TASK MANAGER'S TASK ARRAY IS EMPTY
bool manager_empty(task_manager* manager)
{
    return manager->num_tasks == 0;
}

//ADDS TASK TO TASK ARRAY OF MANAGER
void add_task(task_manager* manager, task* t)
{
    if (manager_full(manager))
    {
        printf("Task manager can't hold anymore tasks\n");
    }
    else
    {
        unsigned int i;

        while (manager->tasks[i] != 0)
        {
            i++;
        }

        manager->tasks[i] = t;
        manager->num_tasks++;
        manager->current_task = manager->tasks[i]->pid;
        t->state = READY;
        printf("Task for instruction at location %p successfully created\n", t->curr_ins);
        ++rem_tasks;
    }
}

void remove_task(task_manager* manager, unsigned int index)
{
    manager->tasks[index] = NULL;
}

//REMOVES TASK FROM MANAGER
//USES BINARY SEARCH TO FIND TASK
void remove_task_by_pid(task_manager* manager, unsigned int pid)
{
    unsigned int rm = task_manager_binary_search(manager, 0, manager->num_tasks, pid);

    if (rm == 0xFF)
    {
        printf("Task with PID %u could not be found in task manager\n", pid);
    }
    else
    {
        manager->tasks[rm] = NULL;
    }
}

//GETS TASK WITH SPECIFIED PID
task* get_task(task_manager* manager, unsigned int pid)
{
    unsigned int result = task_manager_binary_search(manager, 0, manager->num_tasks, pid);

    if (result == 0xFF)
    {
        return NULL;
    }
    else
    {
        return manager->tasks[result];
    }
}

//GETS PID OF TASK AT SPECIFIED LOCATION IN THE TASK MANAGER'S TASK ARRAY
unsigned int inline get_pid(task_manager* manager, unsigned int index)
{
    return manager->tasks[index]->pid;
}

unsigned int get_state(task* t)
{
    switch (t->state)
    {
	    case NEW : return 0x00;
	    case READY : return 0x01;
	    case RUNNING : return 0x02;
	    case WAITING : return 0x03;
        default : return 0xFF;
    }
}

unsigned int inline get_priority(task* t)
{
    return t->priority;
}

//CLEARS TASK MANAGER'S TASK ARRAY
void clear_manager(task_manager* manager)
{
    if (manager_empty(manager))
    {
        printf("Manager is already empty\n");
    }
    else
    {
        for (int i = 0; i < 256; ++i)
        {
            manager->tasks[i] = NULL;
        }

        manager->num_tasks = 0;
        manager->current_task = 0;
    }
}

void kern_stack_switch()
{
    __asm__ __volatile__ ("movl %0, %%esp;" : "=r"(kernel_stack));
}

void set_kern_stack_ptr()
{
    __asm__ __volatile__ ("movl %%esp, %0;" : "=r"(kernel_stack));
}

int set_stack(unsigned int *address)
{
    if (address)
    {
	    __asm__ __volatile__("movl %0, %%esp;" : "=r"(address));
	    return 0;
    }
    else
    {
	    printf("The address that was passed was invalid. Stack pointer has not been changed.\n");
	    return 1;
    }
}

void scheduler(task_manager* manager)
{
    printf("Entering scheduler\n");

    if (!stack_set)
    {
	    //set_kern_stack_ptr();
        stack_set = 1;
    }

    if (manager->num_tasks <= 0)
    {
        printf("No tasks in task manager\n");
        //kern_stack_switch();
        //stack_set = 0;
	    return;
    }
    else
    {
	    //printf("Changing stack pointer to stack at %p for task %x...\n", manager->tasks[manager->current_task]->task_stack, manager->current_task);
	    //unsigned int *p = manager->tasks[manager->current_task]->task_stack->arr;
	    //int err_check = set_stack(p);

	    //if (err_check == 0)
	    //{
	        //printf("Stack pointer set succesfully\n");
	    //}
	    //else
    	//{
	        //printf("Stack pointer change failed\n");
	        //return;
	    //}

	    manager->tasks[manager->current_task]->curr_ins();
	    printf("Task has completed running\n");

	    if (manager->current_task == 0)
        {
            manager->tasks[manager->current_task]->state = RUNNING;
        }
        else
        {
            manager->tasks[manager->current_task - 1]->state = WAITING;
            manager->tasks[manager->current_task]->state = RUNNING;
        }
    }

    remove_task(manager, manager->current_task);
    --rem_tasks;

    if (++manager->current_task >= manager->num_tasks)
    {
        manager->current_task %= manager->num_tasks;
        manager->tasks[manager->num_tasks]->state = WAITING;
    }

    printf("There are %i tasks left.\n", rem_tasks);
}
