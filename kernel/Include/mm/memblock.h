#ifndef _MEMBLOCK_H_
#define _MEMBLOCK_H_

#include <datatype.h>

// 设置成一个递增栈
typedef struct memblock_head
{
    uint64_t all_size;
    size_t use_size;
    size_t desc_count;
    phys_addr stack_top;
    phys_addr stack_end;
} memblock_head_t;

#endif
