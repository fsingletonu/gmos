/*
 * alloc_pages.c
 * (C) Guo Fu
 */
#include <mm/alloc_pages.h>

// 这里一定传入的是可拆分的 这是由上层函数所保证的
bool split(zone_t *zone, size_t order)
{
    page_t *split_page = zone->free_list[order].next;
    zone->free_list[order].next = zone->free_list[order].next->free_ptr.next;
    zone->free_list[order].next->free_ptr.prev = nullptr;
    zone->free_list[order].free_count--;
    size_t logic_page_front = (size_t)Part_PDescaddr_To_PFnnum(zone, split_page);
    size_t logic_page_back = logic_page_front + ((1 << order) >> 1);
    page_t *page_front = (page_t *)Part_Pfnnum_To_PDescaddr(zone, logic_page_front);
    page_t *page_back = (page_t *)Part_Pfnnum_To_PDescaddr(zone, logic_page_back);
    page_front->head = 1;
    page_front->order = order - 1;
    page_front->size = zone->free_list[order - 1].size;
    page_back->head = 1;
    page_back->order = order - 1;
    page_back->size = zone->free_list[order - 1].size;
    page_front->free_ptr.prev = nullptr;
    page_front->free_ptr.next = nullptr;
    page_back->free_ptr.prev = nullptr;
    page_back->free_ptr.next = nullptr;
    page_front->free_ptr.next = page_back;
    page_back->free_ptr.prev = page_front;
    page_t *keep_blk = zone->free_list[order - 1].next;
    zone->free_list[order - 1].next = page_front;
    page_back->free_ptr.next = keep_blk;
    keep_blk->free_ptr.prev = page_back;
    zone->free_list[order - 1].free_count += 2;
}

page_t *alloc_pages(zone_t *zone, size_t order, enum page_type type)
{
    page_t *alloc_blk = nullptr;
    if (zone->free_list[order].free_count > 0)
    {
        alloc_blk = zone->free_list[order].next;
        alloc_blk->type = type;
        alloc_blk->status = PAGE_STATUS_UNFREE;
        zone->free_list[order].next = zone->free_list[order].next->free_ptr.next;
        zone->free_list[order].next->free_ptr.prev = nullptr;
        zone->free_list[order].free_count--;
    }
    else
    {
        size_t i = 1;
        while (zone->free_list[order + i].free_count == 0)
        {
            i++;
            if ((order + i) == MAX_ORDER)
                return nullptr;
        }
        for (size_t j = i; (order + j) != order; j--)
        {
            split(zone, (order + j));
        }
        alloc_blk = zone->free_list[order].next;
        alloc_blk->type = type;
        alloc_blk->status = PAGE_STATUS_UNFREE;
        zone->free_list[order].next = zone->free_list[order].next->free_ptr.next;
        zone->free_list[order].next->free_ptr.prev = nullptr;
        zone->free_list[order].free_count--;
    }
    alloc_blk->free_ptr.prev = nullptr;
    alloc_blk->free_ptr.next = nullptr;
    return alloc_blk;
}

// 判断区域内的兄弟 而不是整个内存区域的兄弟
judge_buddy_struct_t judge_buddy(zone_t *zone, const page_t *blk)
{
    judge_buddy_struct_t keep_mess = {false, false, nullptr};
    phys_addr_t Part_pfn_table_addr = (phys_addr_t)zone->pfn_table_ptr;
    phys_addr_t logic_blk_count = ((phys_addr_t)blk - Part_pfn_table_addr) / sizeof(page_t);
    phys_addr_t logic_blk_addr = logic_blk_count << 12;
    phys_addr_t size = blk->size;
    phys_addr_t logic_buddy_addr = logic_blk_addr ^ size;
    phys_addr_t logic_buddy_count = logic_buddy_addr >> 12;
    phys_addr_t buddy_struct_page = Part_pfn_table_addr + sizeof(page_t) * logic_buddy_count;
    page_t *buddy = (page_t *)buddy_struct_page;
    keep_mess.buddy_PDesc = buddy;
    if (buddy->head == 1 && blk->head == 1 && buddy->order == blk->order && buddy->type == PAGE_TYPE_FREE && buddy->status == PAGE_STATUS_FREE)
    {
        keep_mess.buddy_free = true;
        if ((phys_addr_t)blk < buddy_struct_page)
            keep_mess.front_or_back = false;
        else
            keep_mess.front_or_back = true;
    }
    else
        keep_mess.buddy_free = false;
    return keep_mess;
}

page_t *expand(zone_t *zone, page_t *old_blk_small_addr, page_t *old_blk_big_addr, bool flags)
{
    page_t *keep_buddy = nullptr;
    if (flags == true)
        keep_buddy = old_blk_small_addr;
    else if (flags == false)
        keep_buddy = old_blk_big_addr;
    size_t keep_order = keep_buddy->order;
    if (!keep_buddy->free_ptr.prev && keep_buddy->free_ptr.next)
    {
        zone->free_list[keep_order].next->free_ptr.next->free_ptr.prev = nullptr;
        zone->free_list[keep_order].next = zone->free_list[keep_order].next->free_ptr.next;
    }
    else if (keep_buddy->free_ptr.prev && keep_buddy->free_ptr.next)
    {
        page_t *front = keep_buddy->free_ptr.prev;
        page_t *back = keep_buddy->free_ptr.next;
        front->free_ptr.next = back;
        back->free_ptr.prev = front;
    }
    else if (keep_buddy->free_ptr.prev && !keep_buddy->free_ptr.next)
    {
        keep_buddy->free_ptr.prev->free_ptr.next = nullptr;
    }
    else if (!keep_buddy->free_ptr.prev && !keep_buddy->free_ptr.next)
    {
        zone->free_list[keep_order].next = nullptr;
    }
    zone->free_list[keep_order].free_count--;
    old_blk_small_addr->free_ptr.prev = nullptr;
    old_blk_small_addr->free_ptr.next = nullptr;
    old_blk_big_addr->free_ptr.prev = nullptr;
    old_blk_big_addr->free_ptr.next = nullptr;
    old_blk_small_addr->head = 1;
    old_blk_big_addr->head = 0;
    old_blk_small_addr->order = keep_order + 1;
    old_blk_small_addr->size = old_blk_small_addr->size << 1;
    return old_blk_small_addr;
}

// 由上层保证是一定可释放的
void free_pages(zone_t *zone, page_t *blk)
{
    blk->type = PAGE_TYPE_FREE;
    blk->status = PAGE_STATUS_FREE;
    size_t order = blk->order;
    page_t *keep = blk;
    while (order < MAX_ORDER)
    {
        judge_buddy_struct_t keep_mess = judge_buddy(zone, keep);
        if (keep_mess.buddy_free == true)
        {
            if ((order + 1) != MAX_ORDER)
            {
                page_t *merge_blk = nullptr;
                if (keep_mess.front_or_back == true)
                    merge_blk = expand(zone, keep_mess.buddy_PDesc, blk, true);
                else
                    merge_blk = expand(zone, blk, keep_mess.buddy_PDesc, false);
                page_t *keep_blk = zone->free_list[order].next;
                merge_blk->free_ptr.prev = nullptr;
                merge_blk->free_ptr.next = keep_blk;
                if (keep_blk)
                    keep_blk->free_ptr.prev = merge_blk;
                zone->free_list[order].next = merge_blk;
                order++;
                keep = merge_blk;
            }
            else if (order == MAX_ORDER - 1)
            {
                page_t *keep_blk = zone->free_list[order].next;
                blk->free_ptr.prev = nullptr;
                blk->free_ptr.next = keep_blk;
                if (keep_blk)
                    keep_blk->free_ptr.prev = blk;
                zone->free_list[order].next = blk;
                zone->free_list[order].free_count++;
                order = MAX_ORDER;
            }
        }
        else if (keep_mess.buddy_free == false)
        {
            page_t *keep_blk = zone->free_list[order].next;
            blk->free_ptr.prev = nullptr;
            blk->free_ptr.next = keep_blk;
            if (keep_blk)
                keep_blk->free_ptr.prev = blk;
            zone->free_list[order].next = blk;
            zone->free_list[order].free_count++;
            order = MAX_ORDER;
        }
    }
}