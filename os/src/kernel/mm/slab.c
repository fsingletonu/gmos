/*
 * slab.c
 * (C) Guo Fu
 * 写成可被拆卸的部分 以适应嵌入式设备
 */
#include <mm/slab.h>

void slab_init()
{
    slab_root->occupied = true;
    slab_root->slabs_free = nullptr;
    slab_root->slabs_partial = nullptr;
    slab_root->slabs_full = nullptr;
}

static inline void kmem_cache_object_init(kmem_cache_t *phy_addr)
{
    phy_addr->occupied = false;
    phy_addr->slabs_free = nullptr;
    phy_addr->slabs_partial = nullptr;
    phy_addr->slabs_full = nullptr;
}

static inline void implicit_ptr_init(phys_addr_t *object_start_addr, size_t size, size_t object_size)
{
    phys_addr_t *prev_ptr = (phys_addr_t *)((phys_addr_t)object_start_addr + (phys_addr_t)size);
    phys_addr_t *next_ptr = (phys_addr_t *)((phys_addr_t)object_start_addr + (phys_addr_t)size + sizeof(addr_t));
    phys_addr_t object_prev = (phys_addr_t)object_start_addr - (phys_addr_t)object_size;
    phys_addr_t object_next = (phys_addr_t)object_start_addr + (phys_addr_t)object_size;
    *prev_ptr = object_prev;
    *next_ptr = object_next;
}

static inline void implicit_ptr_set(phys_addr_t *object_start_addr, size_t size, size_t object_size, phys_addr_t addr1, phys_addr_t addr2)
{
    phys_addr_t *prev_ptr = (phys_addr_t *)((phys_addr_t)object_start_addr + (phys_addr_t)size);
    phys_addr_t *next_ptr = (phys_addr_t *)((phys_addr_t)object_start_addr + (phys_addr_t)size + sizeof(addr_t));
    phys_addr_t object_prev = (phys_addr_t)object_start_addr - (phys_addr_t)object_size;
    phys_addr_t object_next = (phys_addr_t)object_start_addr + (phys_addr_t)object_size;
    *prev_ptr = addr1;
    *next_ptr = addr2;
}

phys_addr_t kmem_cache_create(zone_t *zone)
{
    size_t struct_size = sizeof(kmem_cache_t);
kmem_cache_alloc_label:
    if ((!(slab_root->slabs_partial)) && (!(slab_root->slabs_free)))
    {
        page_t *blk = alloc_pages(zone, 1, PAGE_TYPE_SYS_KMEM_CACHE_CAHCE);
        slab_root->slabs_partial = blk;
        phys_addr_t phy_blk = PDescaddr_To_Phypage(zone->mmnode_addr, blk);
        slab_head_t *head = (slab_head_t *)phy_blk;
        head->infree = 0;
        head->inuse = 0;
        head->size = struct_size;
        head->object_size = struct_size + 2 * sizeof(addr_t);
        head->phy_blk = phy_blk;
        for (phys_addr_t i = phy_blk + sizeof(slab_head_t); i < phy_blk + blk->size; i += head->object_size)
        {
            kmem_cache_object_init((kmem_cache_t *)i);
            implicit_ptr_init((phys_addr_t *)i, head->size, head->object_size);
            kmem_cache_t *keep = (kmem_cache_t *)i;
            if (i == phy_blk + sizeof(slab_head_t))
            {
                head->inuse++;
                keep->occupied = true;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, nullptr, nullptr);
                head->use = i;
            }
            else if (i == phy_blk + sizeof(slab_head_t) + (phys_addr_t)head->object_size)
            {
                head->infree++;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, nullptr, (phys_addr_t)i + head->object_size);
                head->free = i;
            }
            else if ((i + (phys_addr_t)head->object_size) > (phy_blk + (phys_addr_t)(blk->size)))
            {
                head->infree++;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, (phys_addr_t)i - head->object_size, nullptr);
            }
            else
            {
                head->infree++;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, (phys_addr_t)i - head->object_size, (phys_addr_t)i + head->object_size);
            }
        }
        return phy_blk + sizeof(slab_head_t);
    }
    else if (slab_root->slabs_partial)
    {
        page_t *blk = slab_root->slabs_partial;
        phys_addr_t phy_blk = PDescaddr_To_Phypage(zone->mmnode_addr, blk);
        slab_head_t *head = (slab_head_t *)phy_blk;
        if (head->infree == 0)
        {
            page_t *keep_partial_blk = blk->free_ptr.next;
            page_t *keep_full_blk = slab_root->slabs_full;
            if (keep_partial_blk)
                slab_root->slabs_partial = keep_partial_blk;
            else
                slab_root->slabs_partial = nullptr;
            blk->free_ptr.next = keep_full_blk;
            if (keep_full_blk)
                keep_full_blk->free_ptr.prev = blk;
            slab_root->slabs_full = blk;
            goto kmem_cache_alloc_label;
        }
        else if (head->infree > 0)
        {
            phys_addr_t current_free = head->free;
            phys_addr_t current_use = head->use;
            phys_addr_t *current_free_prev = (phys_addr_t *)(current_free + struct_size);
            phys_addr_t *current_free_next = (phys_addr_t *)(current_free + struct_size + sizeof(addr_t));
            phys_addr_t *current_use_prev = (phys_addr_t *)(current_use + struct_size);
            phys_addr_t *current_use_next = (phys_addr_t *)(current_use + struct_size + sizeof(addr_t));
            head->free = *current_free_next;
            *current_free_next = current_use;
            *current_use_prev = current_free;
            head->use = current_free;
            return current_free;
        }
    }
}

phys_addr_t kmem_cache_alloc(zone_t *zone, kmem_cache_t *kmem_cache_struct, size_t object_size)
{
slab_cache_alloc_label:
    if ((!(kmem_cache_struct->slabs_partial)) && (!(kmem_cache_struct->slabs_free)))
    {
        page_t *blk = alloc_pages(zone, 1, PAGE_TYPE_SYS_KMEM_CACHE);
        kmem_cache_struct->slabs_partial = blk;
        phys_addr_t phy_blk = PDescaddr_To_Phypage(zone->mmnode_addr, blk);
        slab_head_t *head = (slab_head_t *)phy_blk;
        head->infree = 0;
        head->inuse = 0;
        head->size = object_size;
        head->object_size = object_size + 2 * sizeof(addr_t);
        head->phy_blk = phy_blk;
        for (phys_addr_t i = phy_blk + sizeof(slab_head_t); i < phy_blk + (phys_addr_t)blk->size; i += head->object_size)
        {
            implicit_ptr_init((phys_addr_t *)i, head->size, head->object_size);
            if (i == phy_blk + sizeof(slab_head_t))
            {
                head->inuse++;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, nullptr, nullptr);
                head->use = i;
            }
            else if (i == phy_blk + sizeof(slab_head_t) + (phys_addr_t)head->object_size)
            {
                head->infree++;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, nullptr, (phys_addr_t)i + head->object_size);
                head->free = i;
            }
            else if ((i + (phys_addr_t)head->object_size) > (phy_blk + (phys_addr_t)(blk->size)))
            {
                head->infree++;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, (phys_addr_t)i - head->object_size, nullptr);
            }
            else
            {
                head->infree++;
                implicit_ptr_set((phys_addr_t *)i, head->size, head->object_size, (phys_addr_t)i - head->object_size, (phys_addr_t)i + head->object_size);
            }
        }
        return phy_blk + sizeof(slab_head_t);
    }
    else if (kmem_cache_struct->slabs_partial)
    {
        page_t *blk = kmem_cache_struct->slabs_partial;
        phys_addr_t phy_blk = PDescaddr_To_Phypage(zone->mmnode_addr, blk);
        slab_head_t *head = (slab_head_t *)phy_blk;
        if (head->infree == 0)
        {
            page_t *keep_partial_blk = blk->free_ptr.next;
            page_t *keep_full_blk = slab_root->slabs_full;
            if (keep_partial_blk)
                slab_root->slabs_partial = keep_partial_blk;
            else
                slab_root->slabs_partial = nullptr;
            blk->free_ptr.next = keep_full_blk;
            if (keep_full_blk)
                keep_full_blk->free_ptr.prev = blk;
            slab_root->slabs_full = blk;
            goto slab_cache_alloc_label;
        }
        else if (head->infree > 0)
        {
            phys_addr_t current_free = head->free;
            phys_addr_t current_use = head->use;
            phys_addr_t *current_free_prev = (phys_addr_t *)(current_free + object_size);
            phys_addr_t *current_free_next = (phys_addr_t *)(current_free + object_size + sizeof(addr_t));
            phys_addr_t *current_use_prev = (phys_addr_t *)(current_use + object_size);
            phys_addr_t *current_use_next = (phys_addr_t *)(current_use + object_size + sizeof(addr_t));
            head->free = *current_free_next;
            *current_free_next = current_use;
            *current_use_prev = current_free;
            head->use = current_free;
            return current_free;
        }
    }
}

void kmem_cache_free(zone_t *zone, kmem_cache_t *kmem_cache_struct)
{
}

void slab_free()
{
}