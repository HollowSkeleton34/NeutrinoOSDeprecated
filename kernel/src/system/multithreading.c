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

struct regs* esp;

task_manager* create_task_manager()
{
    task_manager* tm;

    tm->num_tasks = 0;
    tm->current_task = 0xFF;
    return tm;
}

//CREATES A STACK SPECIFICALLY FOR A TASK
stack* create_task_stack()
{
    return stack_create(4096);
}

task* task_create(unsigned int* ins, unsigned int pid, unsigned char priority, void function_ptr())
{
    task* t;

    t->curr_ins = ins;
    t->pid = pid;
    t->priority = priority;
    t->state = NEW;
    t->task_stack = create_task_stack();
    
    //t->(&stack_regs) = (unsigned int)(t->task_stack + 4096 - sizeof(unsigned int));
    
    t->stack_regs->eax = 0;
    t->stack_regs->ebx = 0;
    t->stack_regs->ecx = 0;
    t->stack_regs->edx = 0;
    t->stack_regs->esi = 0;
    t->stack_regs->edi = 0;
    t->stack_regs->eip = (unsigned int)function_ptr;
    t->stack_regs->cs = 0x08;
    t->stack_regs->eflags = 0x202;
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
void add_task_to_manager(task_manager* manager, task* t)
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
    }
}

//REMOVES TASK FROM MANAGER
//USES BINARY SEARCH TO FIND TASK
void remove_task_from_manager(task_manager* manager, unsigned int pid)
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

/**
void scheduler(task_manager* manager, regs* extended_stack_pointer)
{
    regs* esp = extended_stack_pointer;

    if (manager->current_task <= 0)
    {
        __asm__ __volatile("movl %%esp, %%eax;"
                           //"movl %0, esp;"
                           //:"=r"(esp)
                            //);

    }

    if (manager->tasks[current_task]->

    
void scheduling_handler(struct regs* r)
{
*/
