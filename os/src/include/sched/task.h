/*
 * task.h
 * (C) 2026 Guo Fu
 */
#ifndef _TASK_H_
#define _TASK_H_

#include <datatype.h>
#include <mm/memory.h>
#include <sys/fpu.h>

#define SCHED_CFS

#ifdef SCHED_CFS
#include <sched/cfs.h>
#endif

#ifdef SCHED_FCFS
#include <sched/fcfs.h>
#endif

#ifdef SCHED_HRRN
#include <sched/hrrn.h>
#endif

#ifdef SCHED_MFQ
#include <sched/mfq.h>
#endif

#ifdef SCHED_RR
#include <sched/rr.h>
#endif

#ifdef SCHED_SRN
#include <sched/srn.h>
#endif

#define MAX_TASK_SIZE 256

enum process_type
{
    PROCESS_BUILTIN = 0, // embeded
    PROCESS_EXECUTABLE,  // 通用操作系统
};

enum task_status
{
    TASK_STATUS_RUNNING = 0,
    TASK_STATUS_READY,
    TASK_STATUS_SUSPEND,
    TASK_STATUS_BLOCK,
    TASK_STATUS_DEAD
};

// 这个主要是给全进程追踪用的 在另一层，例如：CFS中的entity才是三叉链表
typedef struct task_head
{
    struct task_struct *prev_l; // 可以是左孩子
    struct task_struct *next_r; // 可以是右孩子
} _packed task_head_t;

typedef struct gpr_struct_x86
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
} _packed gpr_struct_x86_t;

typedef struct gpr_struct_x64
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t reg8;
    uint64_t reg9;
    uint64_t reg10;
    uint64_t reg11;
    uint64_t reg12;
    uint64_t reg13;
    uint64_t reg14;
    uint64_t reg15;
} _packed gpr_struct_x64_t;

typedef struct sr_struct_x86_64
{
    uint16_t cs;
    uint16_t ds;
    uint16_t ss;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
} _packed sr_struct_x86_t;

typedef struct mm_struct
{
    uint32_t pgd;
    uint32_t start_code, end_code;
    uint32_t start_data, end_data;
    uint32_t start_args, end_args;
    uint32_t start_stacks;
    page_t *all_pages;
    void (*free)(page_t *all_pages);
} _packed mm_struct_t;

typedef struct task_struct
{
    uint32_t pid;
    uint32_t priority;
    enum task_status status;

#define REG_x86
#ifdef REG_x86
    gpr_struct_x86_t gpr_set;
#endif

#ifndef REG_x86
    gpr_struct_x64_t gpr_set;
#endif

    sr_struct_x86_t sr_set;

    void (*sched)(struct task_struct *root); // 该任务附属的调度器

    mm_struct_t mm;

    fpu_st_t process_fpu;

    task_head_t task_list;
} task_struct_t;

enum sched_type
{
    FCFS = 0,
    SRN,
    RR,
    MFQ,
    HRRN,
    CFS
};

typedef struct sched_class
{
    void (*fcfs_sched)(task_struct_t *root);
    void (*srn_sched)(task_struct_t *root);
    void (*rr_sched)(task_struct_t *root);
    void (*mfq_sched)(task_struct_t *root);
    void (*hrrn_sched)(task_struct_t *root);
    void (*cfs_sched)(task_struct_t *root);
} sched_class_t;
sched_class_t sched;

typedef struct sched_list
{
    task_head_t running_list;
    task_head_t ready_list;
    task_head_t suspend_list;
    task_head_t block_list;
    task_head_t dead_list;
} sched_list_t;
sched_list_t sched_list;

void sched_type_init();
void task_lists_init();
void sched_init();

#endif
