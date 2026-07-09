#ifndef _ARDS_H_
#define _ARDS_H_

#include <datatype.h>

typedef struct bootmem_data
{
    uint32_t ards_buffer;
    uint32_t memory_page;
    uint32_t pt_addr;
    uint32_t kernel_mapping_addr;
    uint32_t kernel_load_addr;
    uint32_t current_bump_ptr;
    uint32_t numa_count;
} _packed bootmem_data_t;
bootmem_data_t bootmem_data;

typedef struct ards
{
    uint64_t base;
    uint64_t limit;
    uint32_t type;
} _packed ards_t;
ards_t *ards_handler;

void bootmem_data_init();

#endif