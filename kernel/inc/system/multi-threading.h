#ifndef __MULTITHREADING__
#define __MULTITHREADING__

#include <containers/stack.h>

//ENUM FOR STATES OF PROCESS
enum status
{
    NEW,
    READY,
    RUNNING,
    WAITING
};

//STRUCTURE FOR PROCESSES
typedef struct task
{
    unsigned int* curr_ins;
    unsigned int pid;
    enum status state;
    unsigned char priority;
    stack* task_stack;
} task;

//STRUCT FOR TASK MANAGER
typedef struct task_manager
{
    task* tasks[256];
    unsigned int num_tasks;
    unsigned int current_task;
} task_manager;

#endif
