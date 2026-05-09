#ifndef _EARLY_ALLOC_H_
#define _EARLY_ALLOC_H_

#include <datatype.h>
#include <BootInfo.h>
#include <lib/string.h>
#include <mm/zone.h>
#include <mm/mapping.h>
#include <mm/memblock.h>

#define MM_SIZE_PRE_UMA (sizeof(mmnode_t) + sizeof(mmzone_t) * ZONE_PRE_NODE)

// 这整个结构在系统的一次周期中只浪费一个地址指针
extern memblock_head_t *memory_new_use_memblock;

typedef struct memblock_entry
{
    uint64_t PhysicalStart;
    uint64_t NumberOfPages;
    uint64_t used_pages;
} memblock_entry_t;

typedef struct mmnode_test
{
    size_t mmnode_id;
    mmzone_t *mmzone[ZONE_PRE_NODE];
    phys_addr pf_base_addr;
    size_t pf_number;
    struct mmnode *prev;
    struct mmnode *next;
    phys_addr mem_map;
} mmnode_test_t;

// 这里是给使用清单分配的页数
#define INIT_MEMBLOCK_REGIONS_PAGES 4

// 只许进 不许出
void insert_memblock(EFI_MEMORY_DESCRIPTOR *desc, size_t used_pages);

void memblock_init(BootInfoHead *Info);

phys_addr early_alloc_pages(BootInfoHead *Info, uint64_t pages);

void struct_page_init(BootInfoHead *Info);

void node_zone_init(BootInfoHead *Info);

void mm_init(BootInfoHead *Info);

#endif
