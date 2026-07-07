#ifndef _NUMA_H_
#define _NUMA_H_

#include <BootInfo.h>
#include <datatype.h>

#define ZONE_PRE_NODE 4

typedef struct mmnode
{
    size_t mmnode_id;
    phys_addr mmzone[ZONE_PRE_NODE];
    phys_addr pf_base_addr;
    size_t pf_number;
    struct mmnode *prev;
    struct mmnode *next;
    phys_addr mem_map;
} mmnode_t;

extern mmnode_t *g_first_node;

void create_node();

#endif
