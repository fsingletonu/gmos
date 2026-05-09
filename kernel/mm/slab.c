#include <mm/slab.h>

kmem_cache_head_t *create_kmem_cache(mmnode_t *node, size_t slab_obj_size, size_t slab_obj_align)
{
    if (slab_obj_size == 0)
        return NULL;

    // 固定分配在normal
    mmzone_t *normal_zone = (mmzone_t *)node->mmzone[2];
    page_t *blk = NULL;
    size_t pages_order = PAGE_16K;

    while (!blk)
    {
        if (pages_order == 0)
            return (kmem_cache_head_t *)blk;

        blk = alloc_pages(normal_zone, pages_order);

        if (!blk && pages_order != 0)
            pages_order -= 1;
    }

    // 这是如果分配成功的情况 这时候的pages就是被分配到的大小 其实也在blk中有备份
    phys_addr header_addr = page2phys(normal_zone, blk);
    setmem((void *)header_addr, (1 << pages_order) * PAGE_SIZE, 0);

    // 标记struct page中的标志位 这个标志位的设置
    for (size_t i = 0; i < (1 << pages_order); i++, blk++)
    {
        blk->flags = FLAGS_KMEMCACHE;
        if (i > 0)
            blk->kmem_cache_head = (kmem_cache_head_t *)header_addr;
    }

    size_t head_size = sizeof(kmem_cache_head_t);
    size_t offset = ALIGN_UP(head_size, 8);
    phys_addr uninit_addr = header_addr + offset;
    size_t stride = ALIGN_UP(sizeof(free_object_t), 8);
    size_t all_objects = 0;

    all_objects = ((1 << pages_order) * PAGE_SIZE - offset) / stride;

    kmem_cache_head_t *header = (kmem_cache_head_t *)header_addr;
    header->head = 1;
    header->pages = (1 << pages_order);
    header->all_objects = all_objects;
    header->uninit_objects = all_objects;
    header->init_per_objects = all_objects / 2;
    header->stride = stride;
    header->slab_obj_size = slab_obj_size;
    header->slab_obj_align = slab_obj_align;
    header->uninit_ptr = (void *)uninit_addr;
    header->node = node;
    header->zone = normal_zone;

    return header;
}

// 个人认为这个就是独立的 所以才用专门的函数释放 其实也就是判断完回收到伙伴系统中 如果不是空 需要为空
bool free_kmem_cache(kmem_cache_head_t *kmem_cache)
{
    size_t inuse = kmem_cache->inuse;
    if (inuse > 0)
        return FALSE;

    // 进行调用伙伴系统
    page_t *blk = (page_t *)phys2page(kmem_cache->node, (phys_addr)kmem_cache);
    return free_pages(kmem_cache->zone, blk);
}

free_object_t *take_kmem_cache_obj(free_object_t **free_list)
{
    free_object_t *obj = NULL;

    if (!(*free_list))
        return obj;

    obj = (free_object_t *)(*free_list);

    *free_list = obj->next;
    if (*free_list)
        (*free_list)->prev = NULL;

    obj->prev = NULL;
    obj->next = NULL;

    return obj;
}

void set_objs_per_init_kmem_cache(kmem_cache_head_t *kmem_cache_blk, size_t val)
{
    kmem_cache_blk->init_per_objects = val;
}

/*
 * 这里传进来的一定是首页头 因为 操作系统交给上层的就是首页头
 * 申请了一个空的kmem_cache_object
 * 这里改为kmem_cache_obj去继承kmem_cache_header中的字段
 */
kmem_cache_t *create_kmem_cache_object(kmem_cache_head_t *kmem_cache_header)
{
    // 这里说明没有可用对象 要初始化 如果kmem_cache全部被占用 就要返回空指针 告诉申请者 根都用完了 该申请了 有的话就直接拆一个下来就行
    if (!kmem_cache_header->free_list)
    {
        if (kmem_cache_header->uninit_objects > 0)
        {
            size_t all_objects = kmem_cache_header->all_objects;
            size_t init_per_objects = kmem_cache_header->init_per_objects;
            size_t uninit_objects = kmem_cache_header->uninit_objects;
            size_t stride = kmem_cache_header->stride;
            size_t init_count = init_per_objects >= uninit_objects ? uninit_objects : init_per_objects;

            free_object_t *obj = (free_object_t *)kmem_cache_header->uninit_ptr;

            // 初始化嵌入式指针
            for (size_t i = 0; i < init_count; i++)
            {
                free_object_t *prev = (free_object_t *)((uint8_t *)obj - stride);
                free_object_t *next = (free_object_t *)((uint8_t *)obj + stride);

                if (uninit_objects != all_objects)
                    obj->prev = prev;
                else
                    obj->prev = NULL;
                if (i == init_count - 1)
                    obj->next = NULL;
                else
                    obj->next = next;

                // 这个就改成5个指针 因为prev next node zone
                size_t stride = ALIGN_UP(kmem_cache_header->slab_obj_size + 4 * sizeof(void *), kmem_cache_header->slab_obj_align);

                obj->header.size = kmem_cache_header->slab_obj_size;
                obj->header.align = kmem_cache_header->slab_obj_align;
                obj->header.stride = stride;
                obj->header.node = kmem_cache_header->node;
                obj->header.zone = kmem_cache_header->zone;

                obj = (free_object_t *)((uint8_t *)obj + stride);
            }

            kmem_cache_header->uninit_objects -= init_count;
            kmem_cache_header->free_list = kmem_cache_header->uninit_ptr;
            if (kmem_cache_header->uninit_objects == 0)
                kmem_cache_header->uninit_ptr = NULL;
            else
                kmem_cache_header->uninit_ptr = (void *)obj;

            kmem_cache_header->inuse++;
            return (kmem_cache_t *)take_kmem_cache_obj((free_object_t **)&kmem_cache_header->free_list);
        }
        else
            return NULL;
    }
    else
    {
        kmem_cache_header->inuse++;
        return (kmem_cache_t *)take_kmem_cache_obj((free_object_t **)&kmem_cache_header->free_list);
    }
}

bool free_kmem_cache_object(kmem_cache_head_t *kmem_cache_blk, kmem_cache_t *kmem_cache_obj)
{
    // 根据对象所属页去定位到空闲链 要加条件判断
    phys_addr obj_page = (phys_addr)kmem_cache_obj & (~0xfff);
    phys_addr blk_end = ((phys_addr)kmem_cache_blk & (~0xfff)) + kmem_cache_blk->pages * PAGE_SIZE;

    if (obj_page >= blk_end)
        return FALSE;

    // 这里一定是该块的对象
    if (!kmem_cache_obj->slabs_full && !kmem_cache_obj->slabs_partial && !kmem_cache_obj->slabs_free)
    {
        free_object_t *keep = (free_object_t *)kmem_cache_blk->free_list;
        free_object_t *insert = (free_object_t *)kmem_cache_obj;

        insert->header.size = 0;
        insert->header.align = 0;
        insert->header.stride = 0;

        if (keep)
        {
            keep->prev = insert;
            insert->next = keep;
            insert->prev = NULL;
        }
        kmem_cache_blk->inuse--;
        kmem_cache_blk->free_list = (void *)kmem_cache_obj;
        return TRUE;
    }
    return FALSE;
}

// 这个是无条件创建slab块
slab_head_t *create_slab(kmem_cache_t *kmem_cache_obj)
{
    // FIFA申请
    if (!kmem_cache_obj)
        return NULL;

    size_t size = kmem_cache_obj->size;
    size_t stride = kmem_cache_obj->stride;

    mmnode_t *node = kmem_cache_obj->node;
    mmzone_t *zone = kmem_cache_obj->zone;

    page_t *blk = NULL;
    size_t pages_order = PAGE_16K;

    while (!blk)
    {
        if (pages_order == 0)
            return (slab_head_t *)blk;

        blk = alloc_pages(zone, pages_order);

        if (!blk && pages_order != 0)
            pages_order -= 1;
    }

    phys_addr header_addr = page2phys(zone, blk);
    setmem((void *)header_addr, (1 << pages_order) * PAGE_SIZE, 0);

    if (kmem_cache_obj->slabs_free)
    {
        slab_head_t *keep = (slab_head_t *)kmem_cache_obj->slabs_free;
        slab_head_t *new = (slab_head_t *)header_addr;
        new->prev = NULL;
        new->next = keep;
        keep->prev = new;
    }
    kmem_cache_obj->slabs_free = (slab_head_t *)header_addr;

    for (size_t i = 0; i < (1 << pages_order); i++, blk++)
    {
        blk->flags = FLAGS_SLAB;
        if (i > 0)
            blk->slab_head = (slab_head_t *)header_addr;
    }

    size_t head_size = sizeof(slab_head_t);
    size_t offset = ALIGN_UP(head_size, 8);
    phys_addr uninit_addr = header_addr + offset;
    size_t all_objects = 0;

    all_objects = ((1 << pages_order) * PAGE_SIZE - offset) / stride;

    slab_head_t *header = (slab_head_t *)header_addr;
    header->head = 1;
    header->pages = (1 << pages_order);
    header->all_objects = all_objects;
    header->uninit_objects = all_objects;
    header->init_per_objects = all_objects / 2;
    header->size = size;
    header->stride = stride;
    header->uninit_ptr = (void *)uninit_addr;
    header->node = node;
    header->zone = zone;
    header->kmem_cache = kmem_cache_obj;

    return header;
}

bool free_slab(slab_head_t *slab)
{
    if (slab->inuse > 0)
        return FALSE;

    page_t *blk = (page_t *)phys2page(slab->node, (phys_addr)slab);
    return free_pages(slab->zone, blk);
}

phys_addr *take_slab_obj(void **free_list, size_t size, size_t stride)
{
    phys_addr *obj = NULL;

    if (!(*free_list))
        return obj;

    obj = (phys_addr *)(*free_list);
    phys_addr *prev = (phys_addr *)((uint8_t *)obj + size);
    phys_addr *next = (phys_addr *)((uint8_t *)prev + sizeof(void *));

    phys_addr *next_obj = (phys_addr *)((uint8_t *)obj + stride);
    phys_addr *free_prev = (phys_addr *)((uint8_t *)next_obj + size);

    *free_list = (void *)next_obj;

    if (*free_list)
        (*free_prev) = NULL;

    *prev = NULL;
    *next = NULL;

    return obj;
}

void set_objs_per_init_slab(slab_head_t *slab, size_t val)
{
    slab->init_per_objects = val;
}

/* 取完就要对链表结构进行改变
 * 1、对slabs_partial取完obj后要判断是否要插入slabs_full
 * 2、slabs_partial可能有三种情况：够用；初始化了一部分；全部初始化且使用
 * 3、slabs_free可能有三种情况：只申请到了没初始化（这个存在于直接调用创建）；初始化但释放了；全部初始化但全释放了
 * 4、释放的free_list一定不为NULL
 * 5、未初始化的一定free_list为NULL
 * 补：（结构）
create_slab_object(kmem_cache_head)
│
├── slabs_partial 存在
│   ├── take_slab_obj（free_list 必然非空）
│   ├── inuse++
│   └── inuse == all_objects → 移到 slabs_full
│
├── slabs_free 存在
│   ├── 移到 slabs_partial
│   ├── free_list == NULL → 惰性初始化 → take_slab_obj
│   └── free_list != NULL → take_slab_obj
│
└── 都没有
    └── create_slab（新 slab 进 slabs_free）
        └── goto label_new_slab_blk
 */
void *create_slab_object(kmem_cache_t *kmem_cache_head)
{
label_new_slab_blk:
    if (!kmem_cache_head)
        return NULL;

    // 这里的slabs_partial是一定有的 因为没有的直接就被放在slabs_full里面了
    if (kmem_cache_head->slabs_partial)
    {
        slab_head_t *slab_blk = (slab_head_t *)kmem_cache_head->slabs_partial;
        slab_blk->inuse++;
        if (slab_blk->inuse == slab_blk->all_objects)
        {
            slab_head_t *keep_full = kmem_cache_head->slabs_full;
            slab_head_t *keep_partial = slab_blk->next;
            slab_blk->prev = NULL;
            slab_blk->next = keep_full;
            if (keep_full)
                keep_full->prev = slab_blk;
            if (keep_partial)
                keep_partial->prev = NULL;
            kmem_cache_head->slabs_full = slab_blk;
            kmem_cache_head->slabs_partial = keep_partial;
        }
        return (void *)take_slab_obj((void **)&slab_blk->free_list, slab_blk->size, slab_blk->stride);
    }
    else if (kmem_cache_head->slabs_free) // 这种情况直接就可以拿
    {
        slab_head_t *slab_blk = kmem_cache_head->slabs_free;
        slab_head_t *keep_free = slab_blk->next;
        slab_head_t *keep_partial = kmem_cache_head->slabs_partial;

        // 先处理链的问题
        slab_blk->prev = NULL;
        slab_blk->next = keep_partial;
        if (keep_partial)
            keep_partial->prev = slab_blk;
        if (keep_free)
            keep_free->prev = NULL;
        kmem_cache_head->slabs_partial = slab_blk;
        kmem_cache_head->slabs_free = keep_free;

        /*
         * 需要延迟初始化的一定在slabs_partial中
         * 有的初始化一部分的在slabs_free中 但没必要初始化 因为浪费初始化开销
         */
        if (!slab_blk->free_list) // 这里一定是未初始化的 因为这里的逻辑和kmem_cache有本质不同 因为这里有slabs_full slabs_partial slabs_free三个队列
        {
            size_t all_objects = slab_blk->all_objects;
            size_t init_per_objects = slab_blk->init_per_objects;
            size_t uninit_objects = slab_blk->uninit_objects;
            size_t init_count = init_per_objects >= uninit_objects ? uninit_objects : init_per_objects;
            size_t size = slab_blk->size;
            size_t stride = slab_blk->stride;

            phys_addr *obj = (phys_addr *)slab_blk->uninit_ptr;

            for (size_t i = 0; i < init_count; i++)
            {
                phys_addr *prev_obj = (phys_addr *)((uint8_t *)obj - stride);

                phys_addr *obj_prev = (phys_addr *)((uint8_t *)obj + size);
                phys_addr *obj_next = (phys_addr *)((uint8_t *)obj_prev + sizeof(void *));
                phys_addr *obj_node = (phys_addr *)((uint8_t *)obj_next + sizeof(void *));
                phys_addr *obj_zone = (phys_addr *)((uint8_t *)obj_node + sizeof(void *));

                phys_addr *next_obj = (phys_addr *)((uint8_t *)obj + stride);

                if (uninit_objects != all_objects)
                    *obj_prev = (size_t)prev_obj;
                else
                    *obj_prev = NULL;
                if (i == init_count - 1)
                    *obj_next = NULL;
                else
                    *obj_next = (size_t)next_obj;

                *obj_node = (size_t)slab_blk->node;
                *obj_zone = (size_t)slab_blk->zone;

                obj = next_obj;
            }

            slab_blk->uninit_objects -= init_count;
            slab_blk->free_list = slab_blk->uninit_ptr;

            if (slab_blk->uninit_objects == 0)
                slab_blk->uninit_ptr = NULL;
            else
                slab_blk->uninit_ptr = (void *)obj;

            slab_blk->inuse++;
            return (void *)take_slab_obj((void **)&slab_blk->free_list, slab_blk->size, slab_blk->stride);
        }
        else // 这里是一定初始化过且是释放了的 释放了的且不在free_list中 这种情况不存在
        {
            slab_blk->inuse++;
            return (void *)take_slab_obj((void **)&slab_blk->free_list, slab_blk->size, slab_blk->stride);
        }
    }
    else // 这里就要直接申请了 这里都没有可用的
    {
        create_slab(kmem_cache_head);
        goto label_new_slab_blk;
    }
}

void remove_slab(slab_head_t *slab_blk)
{
    if (!slab_blk->prev && slab_blk->next)
    {
        slab_blk->next->prev = NULL;
    }
    else if (slab_blk->prev && slab_blk->next)
    {
        slab_blk->next->prev = slab_blk->prev;
        slab_blk->prev->next = slab_blk->next;
    }
    else if (slab_blk->prev && !slab_blk->next)
    {
        slab_blk->prev->next = NULL;
    }
}

// 根据node、zone及页基地址定位到struct page 再根据struct page找到slab_head_t
bool free_slab_object(void *obj, size_t size)
{
    // 定位到struct page
    phys_addr *prev = (phys_addr *)((uint8_t *)obj + size);
    phys_addr *next = (phys_addr *)((uint8_t *)prev + sizeof(void *));
    phys_addr *node_ptr = (phys_addr *)((uint8_t *)next + sizeof(void *));
    phys_addr *zone_ptr = (phys_addr *)((uint8_t *)node_ptr + sizeof(void *));
    mmnode_t *node = (mmnode_t *)(*node_ptr);
    mmzone_t *zone = (mmzone_t *)(*zone_ptr);

    phys_addr base_addr = (phys_addr)obj & (~0xfff);

    page_t *page = (page_t *)phys2page(node, base_addr);

    if (page->flags != FLAGS_SLAB)
        return FALSE;

    slab_head_t *slab_blk = NULL;

    if (page->head == 1)
        slab_blk = (slab_head_t *)base_addr;
    else
        slab_blk = (slab_head_t *)page->slab_head;

    // 先把对象插进去
    void *keep = slab_blk->free_list;
    phys_addr *keep_prev = (phys_addr *)((uint8_t *)keep + slab_blk->size);
    phys_addr *keep_next = (phys_addr *)((uint8_t *)keep_prev + sizeof(void *));

    *prev = NULL;
    *next = (phys_addr)keep;
    if (keep)
        *keep_prev = (phys_addr)obj;
    slab_blk->free_list = (void *)obj;

    slab_blk->inuse--;

    kmem_cache_t *kmem_cache_head = slab_blk->kmem_cache;
    /*
     * 这里要考虑到被释放的slab块是在链中的哪个位置
     * slab_blk就是被拆下的
     *
     */

    if (slab_blk->inuse == slab_blk->all_objects - 1)
    {
        if (kmem_cache_head->slabs_full == slab_blk)
            kmem_cache_head->slabs_full = slab_blk->next;
        remove_slab(slab_blk);
        slab_head_t *next_slab = kmem_cache_head->slabs_partial;
        if (next_slab)
            next_slab->prev = slab_blk;
        slab_blk->next = next_slab;
        kmem_cache_head->slabs_partial = slab_blk;
    }
    else if (slab_blk->inuse == 0)
    {
        if (kmem_cache_head->slabs_partial == slab_blk)
            kmem_cache_head->slabs_partial = slab_blk->next;
        remove_slab(slab_blk);
        slab_head_t *next_slab = kmem_cache_head->slabs_free;
        if (next_slab)
            next_slab->prev = slab_blk;
        slab_blk->next = next_slab;
        kmem_cache_head->slabs_free = slab_blk;
    }

    return TRUE;
}