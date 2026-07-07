#ifndef _SMP_H_
#define _SMP_H_

#include <datatype.h>
#include <init/acpi.h>
#include <x86_64/tss.h>

#define MAX_CPUS 16

typedef struct per_cpu
{
    uint32_t cpu_index;    // 逻辑索引
    uint32_t apic_id;      // 硬件 APIC ID
    tss_t *tss;            // 本核 TSS 指针
    uint64_t kernel_stack; // 本核内核栈顶
} per_cpu_t;

size_t get_core_count_init();
void local_cpu_init();

#endif
