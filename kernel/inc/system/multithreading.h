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
    void (*curr_ins)();
    unsigned int pid;
    enum status state;
    unsigned int priority;
    stack* task_stack;
} task;

//STRUCT FOR TASK MANAGER
typedef struct task_manager
{
    task* tasks[256];
    unsigned int num_tasks;
    unsigned int current_task;
} task_manager;


//GLOBALS
task_manager* system_task_manager;

//FUNCTIONS
task_manager* create_task_manager();
stack* create_task_stack();
task* task_create(unsigned int pid, unsigned int priority, void (*function_ptr)());
bool manager_full(task_manager* manager);
bool manager_empty(task_manager* manager);
void add_task_to_manager(task_manager* manager, task* t);
void remove_task_from_manager(task_manager* manager, unsigned int pid);
task* get_task(task_manager* manager, unsigned int pid);
unsigned int inline get_pid(task_manager* manager, unsigned int index);
unsigned int get_state(task* t);
unsigned int inline get_priority(task* t);
void clear_manager(task_manager* manager);
extern void do_jmp();
extern void pushy();
extern void poppy();

#endif
