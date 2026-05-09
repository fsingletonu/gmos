#include <mm/zone.h>

// 这个页帧号填的是相对于整体的offset
phys_addr pf2page(mmzone_t *zone, size_t number)
{
    phys_addr mem_map_addr = ((mmnode_t *)zone->mmnode_addr)->mem_map;
    phys_addr page_addr = NULL;
    page_addr = mem_map_addr + (zone->start_pf_number + number) * sizeof(page_t);
    return page_addr;
}

phys_addr phys2page(mmnode_t *node, phys_addr addr)
{
    phys_addr mem_map_addr = node->mem_map;
    uint64_t offset = (addr & (~0xfff)) >> 12;
    phys_addr page_addr = mem_map_addr + offset * sizeof(page_t);
    return page_addr;
}

phys_addr page2phys(mmzone_t *zone, page_t *page)
{
    phys_addr mem_map_addr = ((mmnode_t *)zone->mmnode_addr)->mem_map;
    phys_addr offset = ((phys_addr)page - mem_map_addr) / sizeof(page_t);
    phys_addr addr = offset << 12;
    return addr;
}

// 这个根据node去找到起始地址 伙伴系统初始化 使用尾插法 伙伴系统的释放使用头插法
void create_one_zone_buddy(mmzone_t *zone)
{
    size_t pf_start = zone->start_pf_number;
    int pf_number = zone->pf_number;
    page_t *page = (page_t *)pf2page(zone, pf_start);
    size_t register_page = 0;

    // 每个阶数都要维护尾指针
    page_t *tail[MAX_ORDER] = {0};
    while (register_page < pf_number)
    {
        // 记录有多少连续空闲
        int keep_count = 0;

        // 记录有多少连续占用
        int occupied = 0;

        while (page->flags == FLAGS_FREE)
        {
            keep_count++;
            page++;
        }

        while (page->flags == FLAGS_RESERVED)
        {
            occupied++;
            page++;
        }

        register_page += (keep_count + occupied);

        page -= (keep_count + occupied);

        if (keep_count > 0)
        {
            for (int i = MAX_ORDER - 1; i >= 0; i--)
            {
                while (keep_count >= (1 << i))
                {
                    page->head = 1;
                    page->order = i;
                    page->prev = NULL;
                    page->next = NULL;

                    zone->free_list[i].free_number++;

                    // 尾插法
                    if (!(zone->free_list[i].next))
                        zone->free_list[i].next = page;
                    else
                    {
                        tail[i]->next = page;
                        page->prev = tail[i];
                    }
                    tail[i] = page;
                    keep_count -= (1 << i);
                    page += (1 << i);
                }
            }
        }
        page += occupied;
    }
}

void create_zone_buddy(mmnode_t *node)
{
    for (size_t i = 0; i < ZONE_PRE_NODE; i++)
    {
        mmzone_t *zone = (mmzone_t *)node->mmzone[i];
        create_one_zone_buddy(zone);
    }
}

void buddy_system_init(mmnode_t *node)
{
    size_t start_pf_number = 0;
    size_t pf_number = 0;
    phys_addr mmnode_addr = (phys_addr)node;
    for (size_t i = 0; i < ZONE_PRE_NODE; i++)
    {
        mmzone_t *zone = (mmzone_t *)((uint8_t *)node->mmzone[i]);
        switch (i)
        {
        case ZONE_DMA:
            pf_number = ZONE_DMA_PAGE;
            break;

        case ZONE_DMA32:
            pf_number = ZONE_DMA32_PAGE;
            break;

        case ZONE_NORMAL:
            pf_number = node->pf_number - ZONE_DMA_PAGE - ZONE_DMA32_PAGE;
            break;

        case ZONE_RESERVED:
            pf_number = ZONE_RESERVED_PAGE;
            break;

        default:
            break;
        }
        zone->start_pf_number = start_pf_number;
        zone->pf_number = pf_number;
        zone->mmnode_addr = (phys_addr)node;
        start_pf_number += pf_number;
    }
}

void mmzone_init(mmnode_t *node)
{
    buddy_system_init(node);
    create_zone_buddy(node);
}

// 这里是被拆分的order 这里的可拆分由上层函数已经保证
void split(mmzone_t *zone, size_t order)
{
    // 分离操作
    zone->free_list[order].free_number--;
    size_t free_number = zone->free_list[order].free_number;
    page_t *block = zone->free_list[order].next;
    if (free_number == 0)
        zone->free_list[order].next = NULL;
    else
    {
        zone->free_list[order].next = block->next;
        zone->free_list[order].next->prev = NULL;
    }

    page_t *first_block = block;
    page_t *second_block = block + (1 << (order - 1));

    first_block->head = 1;
    first_block->order = order - 1;
    second_block->head = 1;
    second_block->order = order - 1;

    first_block->next = second_block;
    second_block->prev = first_block;
    first_block->prev = NULL;
    second_block->next = NULL;

    // 插入操作
    size_t new_free_number = zone->free_list[order - 1].free_number;
    if (new_free_number == 0)
        zone->free_list[order - 1].next = first_block;
    else
    {
        page_t *keep_block = zone->free_list[order - 1].next;
        zone->free_list[order - 1].next = first_block;
        second_block->next = keep_block;
        keep_block->prev = second_block;
    }
    zone->free_list[order - 1].free_number += 2;
}

// 这个函数也是由上层保证一定能取到
page_t *take_page_from_list(mmzone_t *zone, size_t order)
{
    page_t *page = NULL;
    zone->free_list[order].free_number--;
    page = zone->free_list[order].next;

    if (!page->next)
        zone->free_list[order].next = NULL;
    else
    {
        zone->free_list[order].next = page->next;
        page->next->prev = NULL;
    }

    page->prev = NULL;
    page->next = NULL;
    return page;
}

// 分配的入口是zone 这可以为后续的SMP提供便利
page_t *alloc_pages(mmzone_t *zone, size_t order)
{
    page_t *page = NULL;

    // 这里还是做一下限制吧
    if (order > MAX_ORDER - 1)
        return page;

    // 先找有没有符合的 找到最小的order符合的向下拆
    if (zone->free_list[order].free_number > 0)
    {
        return take_page_from_list(zone, order);
    }
    else
    {
        // 记录分裂的距离 以便回裂
        size_t split_count = 0;

        for (size_t i = 1; i < MAX_ORDER - order; i++)
        {
            size_t block_number = zone->free_list[order + i].free_number;
            if (block_number == 0)
                split_count++;
            else
                break;

            if ((order + i) == MAX_ORDER - 1 && block_number == 0)
                return page;
        }

        // 回裂
        for (size_t i = split_count + order; i > order; i--)
            split(zone, i);

        return take_page_from_list(zone, order);
    }
}

// 如果有的话 就返回伙伴struct page地址；如果没有 那就返回NULL （这个也可以作为循环结束的标志）
page_t *get_buddy(mmzone_t *zone, page_t *blk)
{
    page_t *buddy = NULL;
    phys_addr buddy_addr = NULL;
    phys_addr blk_addr = NULL;
    size_t blk_order = blk->order;
    size_t blk_size = (1 << blk_order) * 4096;

    blk_addr = page2phys(zone, blk);
    buddy_addr = blk_addr ^ blk_size;

    buddy = (page_t *)phys2page((mmnode_t *)zone->mmnode_addr, buddy_addr);

    if (blk->head == 1 && buddy->head == 1 && blk->order == buddy->order)
        return buddy;

    return NULL;
}

/*
 * 传入的能不能合并是由上层保证的 这个flags是用来判断被释放的是哪一个 原本在队列中的是哪一个 0代表blk1 1代表blk2
 * 还有一点就是这里不要插入了 只给合并结果 因为不知道还需要合并几次
 *
 */
page_t *expend(mmzone_t *zone, page_t *blk1, page_t *blk2, bool flags)
{
    // 这个量既用来中间 又用来标注谁是在队列的那个
    page_t *temp = NULL;
    if ((phys_addr)blk1 > (phys_addr)blk2)
    {
        temp = blk2;
        blk2 = blk1;
        blk1 = temp;
        flags = !flags;
    }

    if (flags == FALSE)
        temp = blk1;
    else
        temp = blk2;

    size_t keep_order = blk1->order;

    blk1->head = 1;
    blk1->order++;
    blk2->head = 0;
    blk2->order = 0;

    /*
     * 指针处理得分情况（原本在队列中的）：
     * 1、当节点在最左
     * 2、当节点在中间
     * 3、当节点在最右
     */
    if (!temp->prev && temp->next)
    {
        zone->free_list[keep_order].next = temp->next;
        temp->next->prev = NULL;
    }
    else if (temp->prev && temp->next)
    {
        temp->next->prev = temp->prev;
        temp->prev->next = temp->next;
    }
    else if (temp->prev && !temp->next)
    {
        temp->prev->next = NULL;
    }
    temp->prev = NULL;
    temp->next = NULL;

    zone->free_list[keep_order].free_number--;

    return blk1;
}

bool insert_page_into_list(mmzone_t *zone, page_t *blk)
{
    size_t order = blk->order;
    if (!zone->free_list[order].next)
        zone->free_list[order].next = blk;
    else
    {
        blk->next = zone->free_list[order].next;
        zone->free_list[order].next->prev = blk;
        zone->free_list[order].next = blk;
    }
    zone->free_list[order].free_number++;
    return TRUE;
}

// 由上层保证一定是可释放的
bool free_pages(mmzone_t *zone, page_t *blk)
{
    bool status = FALSE;
    size_t init_order = blk->order;
    page_t *buddy = NULL;

    while (init_order < MAX_ORDER - 1)
    {
        buddy = get_buddy(zone, blk);
        if (buddy)
        {
            blk = expend(zone, blk, buddy, TRUE);
            init_order++;
        }
        else
            break;
    }

    // 插入逻辑
    return insert_page_into_list(zone, blk);
}