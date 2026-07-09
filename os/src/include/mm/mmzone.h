/*
 * mmzone.h
 * (C) 2026 Guo Fu
 */
#ifndef _MMZONE_H_
#define _MMZONE_H_

#include <datatype.h>
#include <blk_drv/dma.h>
#include <mm/numa.h>

#define MAX_ORDER 11

#define ZONE_TYPE_NORMAL_ON
#define ZONE_TYPE_DMA32_ON
#define ZONE_TYPE_DMA_ON

// 单位是 页
#define ZONE_TYPE_LOW_END_SIZE 1024
#define ZONE_TYPE_DMA_SIZE 1024
#define ZONE_TYPE_DMA32_SIZE 0
#define ZONE_TYPE_NORMAL_SIZE bootmem_data.memory_page - ZONE_TYPE_LOW_END_SIZE - ZONE_TYPE_DMA_SIZE - ZONE_TYPE_DMA32_SIZE

#define ZONE_COUNT 4

enum zone_types
{
    ZONE_TYPE_LOW_END,
#ifdef ZONE_TYPE_DMA_ON
    ZONE_TYPE_DMA,
#endif

#ifdef ZONE_TYPE_DMA32_ON
    ZONE_TYPE_DMA32,
#endif

#ifdef ZONE_TYPE_NORMAL_ON
    ZONE_TYPE_NORMAL,
#endif

};

enum page_type
{
    PAGE_TYPE_SYS_PDE = 0,
    PAGE_TYPE_SYS_PTE,
    PAGE_TYPE_USR_PDE,
    PAGE_TYPE_USR_PTE,
    PAGE_TYPE_SYS_DATA,
    PAGE_TYPE_SYS_CODE,
    PAGE_TYPE_SYS_PDESC,
    PAGE_TYPE_SYS_BITMAP,
    PAGE_TYPE_USR_DATA,
    PAGE_TYPE_USR_CODE,
    PAGE_TYPE_FREE,
    PAGE_TYPE_RESERVE,
    PAGE_TYPE_SYS_BUDDY,
    PAGE_TYPE_SYS_CODE_DATA,
    PAGE_TYPE_SYS_KMEM_CACHE,
    PAGE_TYPE_SYS_KMEM_CACHE_CAHCE,
    PAGE_TYPE_SYS_SLAB,
    PAGE_TYPE_TASK_RUNNING_LIST,
    PAGE_TYPE_TASK_READY_LIST,
    PAGE_TYPE_TASK_SUSPEND_LIST,
    PAGE_TYPE_TASK_BLOCK_LIST,
    PAGE_TYPE_TASK_DEAD_LIST
};

enum page_status
{
    PAGE_STATUS_UNFREE = 0,
    PAGE_STATUS_FREE
};

typedef struct free_head
{
    struct page *prev;
    struct page *next;
} _packed free_head_t;

typedef struct page
{
    uint8_t head; // 是否是头页 如果头页 则free_area的order字段生效 0 为非头页
    uint32_t order;
    uint32_t size; // 块的大小
    enum page_type type;
    enum page_status status;
    uint32_t ref_count;
    uint32_t mmap_count;
    free_head_t free_ptr;
} _packed page_t;

typedef struct free_area
{
    uint32_t free_count;
    uint32_t order; // 阶数
    uint32_t size;  // 块的大小
    page_t *next;
} _packed free_area_t;

typedef struct zone
{
    enum zone_types type;
    uint32_t phy_start_pfn;
    uint32_t spanned_pages; // 一共包括的物理页个数
    uint32_t present_pages; // 实际可以被使用的页
    phys_addr_t mmnode_addr;
    phys_addr_t pfn_table_ptr;
    uint32_t pfn_count;
    free_area_t free_list[MAX_ORDER];
    struct zone *next;
} _packed zone_t;

phys_addr_t Global_Pfnnum_To_PDescaddr(mmnode_t *node, size_t pfnnum);
phys_addr_t Part_Pfnnum_To_PDescaddr(zone_t *zone, size_t pfnnum);
size_t Global_PDescaddr_To_PFnnum(mmnode_t *node, page_t *page);
size_t Part_PDescaddr_To_PFnnum(zone_t *zone, page_t *page);

// 这个用zone内的node指针
phys_addr_t PDescaddr_To_Phypage(phys_addr_t node_pfn_base, page_t *page);
phys_addr_t Phypage_To_PDescaddr(zone_t *zone, phys_addr_t page);
void mmnode_pfn_init();
void mmzone_init();

#endif
