/*
 * numa.h
 * (C) 2026 Guo Fu
 */
#ifndef _NUMA_H_
#define _NUMA_H_

#include <datatype.h>
#include <init/acpi.h>
#include <init/ards.h>
#include <init/pci.h>

enum node_status
{
    N_NORMAL_MEMORY = 0, // 普通内存，个人认为用于UMA架构
    N_NOT_CPU,           // 未被CPU占用
    N_CPU                // 被CPU占用
};

typedef struct mmnode
{
    uint32_t mmnode_id;
    enum node_status status;
    uint64_t persent_pages_num;
    uint64_t spanned_pages_num;
    phys_addr_t low_end_zone;
    phys_addr_t dma_zone;
    phys_addr_t dma32_zone;
    phys_addr_t normal_zone;
    phys_addr_t pfn_table_ptr;
    uint32_t pfn_count;
    struct mmnode *next; // 其它的node
} _packed mmnode_t;
mmnode_t *first_node;

void bootmem_data_init();
void mmnode_init();

#endif
