#include <algorithms/algorithms.h>

unsigned int task_manager_binary_search(task_manager* manager, unsigned int start, unsigned int end, unsigned int pid)
{
    while (start <= end)
    {
        unsigned int middle = start + (end - 1) / 2;

        if (manager->tasks[middle]->pid == pid)
        {
            return middle;
        }
        
        if (manager->tasks[middle]->pid < pid)
        {
            start = middle + 1;
        }
        else
        {
            end = middle - 1;
        }
    }

    return 0xFF;
}

unsigned int binary_search(unsigned int arr[], unsigned int start, unsigned int end, unsigned int val)
{
    while (start <= end)
    {
        int middle = start + (end - 1) / 2;

        if (arr[middle] == val)
        {
            return middle;
        }
        
        if (arr[middle] < val)
        {
            start = middle + 1;
        }
        else
        {
            end = middle - 1;
        }
    }

    return 0xFF;
}
