/*
 * 这里基础数据类型就是要连续空间 程序可以用分散的空间（用MMU逻辑连续化就行）
 * 并且 slab的块内的页可以不连续
 * 如果实在需要连续 但没有内存 会根据申请的轻重缓急 去启用虚拟内存
 */
#ifndef _ZONE_H_
#define _ZONE_H_

#include <datatype.h>
#include <lib/string.h>
#include <mm/numa.h>
#include <mm/page.h>

/*
 * ZONE区域暂时划分为：
 * 1、ZONE_DMA
 * 2、ZONE_DMA32
 * 3、ZONE_NORMAL
 * 4、ZONE_RESERVED
 * 前面两个是为了预留 如果不考虑的话 直接宏定义为 0
 */

#define MAX_ORDER 11

#define ZONE_DMA_PAGE 0
#define ZONE_DMA32_PAGE 0
#define ZONE_RESERVED_PAGE 0

typedef enum
{
    ZONE_DMA = 0,
    ZONE_DMA32,
    ZONE_NORMAL,
    ZONE_RESERVED
} ZONE_TYPE;

typedef enum
{
    ALLOC_CONTIGUOUS,
    ALLOC_HIGHPRIO
} ALLOC_TYPE;

typedef struct page_free_list
{
    size_t free_number;
    page_t *next;
} page_free_list_t;

typedef struct mmzone
{
    // 这个lock先预留出来 之后改为LOCK
    bool lock;
    size_t start_pf_number;
    size_t pf_number;
    phys_addr mmnode_addr;
    page_free_list_t free_list[MAX_ORDER];
} mmzone_t;

phys_addr pf2page(mmzone_t *zone, size_t number);

phys_addr phys2page(mmnode_t *node, phys_addr addr);

phys_addr page2phys(mmzone_t *zone, page_t *page);

void mmzone_init(mmnode_t *node);

page_t *alloc_pages(mmzone_t *zone, size_t order);

page_t *get_buddy(mmzone_t *zone, page_t *blk);

bool free_pages(mmzone_t *zone, page_t *blk);

#endif
