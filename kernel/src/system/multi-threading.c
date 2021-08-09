#include <system/multi-threading.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>
#include <algorithms.h>
#include <sys_kern.h>
#include <containers/stack.h>

//CREATES A STACK SPECIFICALLY FOR A TASK
stack* inline create_task_stack()
{
    return stack_create(4096);
}

task* task_create(unsigned int* ins, unsigned int pid, unsigned char priority)
{
    task* t;

    t->curr_ins = ins;
    t->pid = pid;
    t->priority = priority;
    t->state = NEW;
    t->task_stack = create_task_stack();
    
    return t;
}

//CHECKS IF THE TASK MANAGER'S TASK ARRAY IS FULL
bool inline manager_full(task_manager* manager)
{
    return manager->num_tasks == 256;
}

//CHECKS IF TASK MANAGER'S TASK ARRAY IS EMPTY
bool inline manager_empty(task_manager* manager)
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
        int i;
    
        while (manager[i] != 0)
        {
            i++;
        }
    
        manager[i] = t;
        manager->num_tasks++;
        manager->current_task = manager->tasks[i]->pid;
        t->state = READY;
        printf("Task for instruction at location %p successfully created\n", t.curr_ins);
    }
}

//REMOVES TASK FROM MANAGER
//USES BINARY SEARCH TO FIND TASK
void remove_task_from_manager(task_manager* manager, unsigned int pid)
{
    unsigned int rm = task_mamager_binary_search(manager->tasks, 0, manager->num_tasks, pid);
    
    if (rm == 0xFF);
    {
        printf("Task with PID %u could not be found in task manager\n", pid);
    }
    else
    {
        manager->tasks[rm] = NULL;
    }
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
            manager[i] = NULL;
        }

        manager->num_tasks = 0;
        manager->current_task = 0;
    }
}

