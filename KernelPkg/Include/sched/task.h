#ifndef _TASK_H_
#define _TASK_H_

#include <datatype.h>
#include <syshd/fpu.h>

typedef enum
{
    TASK_STATUS_RUNNING = 0,
    TASK_STATUS_READY,
    TASK_STATUS_SUSPEND,
    TASK_STATUS_BLOCK,
    TASK_STATUS_DEAD
} TASK_STAUS;

#pragma pack(push, 1)
typedef struct gr_x86
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
} gr_x86_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct sr_x86_64
{
    uint16_t cs;
    uint16_t ds;
    uint16_t ss;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
} sr_x86_64_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct gr_x64
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
} gr_x64_t;
#pragma pack(pop)

typedef struct mm
{

} mm_t;

typedef struct task
{
    size_t pid;
    size_t prio;
    TASK_STAUS status;

    /*
     * 当使用 -m32 时，GCC 会自动定义 __i386__（x86-32）
     * 当使用 -m64 时，GCC 会自动定义 __x86_64__（x86-64）
     */

#if defined(__i386__)
    gr_x86_t gr;
#elif defined(__x86_64__)
    gr_x64_t gr;
#endif

    sr_x86_64_t sr;

    fpu_x87_t fpu_x87;

} task_t;

#endif
