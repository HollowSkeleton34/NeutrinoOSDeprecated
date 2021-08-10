#ifndef __MULTI-THREADING__
#define __MULTI-THREADING__

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
    struct regs* stack_regs;
    stack* task_stack;
} task;

//STRUCT FOR TASK MANAGER
typedef struct task_manager
{
    task* tasks[256];
    unsigned int num_tasks;
    unsigned int current_task;
} task_manager;

inline stack* create_task_stack();
task* task_create(unsigned int* ins, unsigned int pid, unsigned char priority, void function_ptr());
bool inline manager_full(task_manager* manager);
bool inline manager_empty(task_manager* manager);
void add_task_to_manager(task_manager* manager, task* t);
void remove_task_from_manager(task_manager* manager, unsigned int pid);
task* get_task(task_manager* manager, unsigned int pid);
unsigned int inline get_pid(task_manager* manager, unsigned int index);
void clear_manager(task_manager* manager);

#endif
