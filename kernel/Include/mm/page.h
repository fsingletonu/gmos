#ifndef _PAGE_H_
#define _PAGE_H_

#include <datatype.h>

#define PAGE_SIZE 4096

typedef enum
{
    FLAGS_FREE = 0,
    FLAGS_RESERVED = (1 << 0),
    FLAGS_BUDDY = (1 << 1),
    FLAGS_KMEMCACHE = (1 << 2),
    FLAGS_SLAB = (1 << 3)
} PAGE_FLAGS;

struct kmem_cache_head;
struct slab_head;

typedef struct page
{
    bool head;
    PAGE_FLAGS flags;
    size_t order;
    size_t ref_count;
    size_t mmap_count;

    union
    {
        struct page *prev;
        struct kmem_cache_head *kmem_cache_head; // 分配给 slab 后指向所属 kmem_cache_head
        struct slab_head *slab_head;
    };

    // 这个指针可以复用为非连续页指针 也就是连续分配
    struct page *next;
} page_t;

#endif
