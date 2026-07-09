/*
 * slab.h
 * (C) 2026 Guo Fu
 */
#ifndef _SLAB_H_
#define _SLAB_H_

#include <datatype.h>
#include <mm/alloc_pages.h>

// 根据occupied来判断指针的类型 指针是动态生成指针
typedef struct kmem_cache
{
    bool occupied;
    page_t *slabs_full;
    page_t *slabs_partial;
    page_t *slabs_free;
} _packed kmem_cache_t;

typedef struct cache_head
{
    struct page *prev;
    struct page *next;
} _packed cache_head_t;

typedef struct slab_head
{
    size_t size;
    size_t object_size;
    size_t align;
    size_t inuse;
    size_t infree;
    phys_addr_t free;
    phys_addr_t use;
    phys_addr_t phy_blk;
    uint8_t bitmap[]; // 可选选项 看需要进行选择 需要有两种实现方式
} _packed slab_head_t;
#define SLAB_HEAD_SIZE sizeof(slab_head_t)

/* slab管理的入口
 * slab_root下面挂着的是kmem_cache_cache
 * kmem_cache_cache块中存放着kmem_cache
 * kmem_cache下面挂着的是slab_head块
 */
kmem_cache_t *slab_root;

void slab_init();
phys_addr_t kmem_cache_create(zone_t *zone);
phys_addr_t kmem_cache_alloc(zone_t *zone, kmem_cache_t *kmem_cache_struct, size_t object_size);
void kmem_cache_free(zone_t *zone, kmem_cache_t *kmem_cache_struct);
void slab_free();

#endif
