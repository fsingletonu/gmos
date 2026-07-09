#include <sched/task.h>

void sched_type_init()
{
}

void task_lists_init()
{
    sched_list.running_list.prev_l = nullptr;
    sched_list.running_list.next_r = nullptr;
    sched_list.ready_list.prev_l = nullptr;
    sched_list.ready_list.next_r = nullptr;
    sched_list.suspend_list.prev_l = nullptr;
    sched_list.suspend_list.next_r = nullptr;
    sched_list.block_list.prev_l = nullptr;
    sched_list.block_list.next_r = nullptr;
    sched_list.dead_list.prev_l = nullptr;
    sched_list.dead_list.next_r = nullptr;
}

void sched_init()
{
    task_lists_init();
}