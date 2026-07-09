/*
 * mmzone.c
 * (C) 2026 Guo Fu
 */
#include <mm/mmzone.h>

phys_addr_t Global_Pfnnum_To_PDescaddr(mmnode_t *node, size_t pfnnum)
{
    phys_addr_t PDescaddr = nullptr;
    phys_addr_t Global_pfn_table_addr = node->pfn_table_ptr;
    PDescaddr = Global_pfn_table_addr + sizeof(page_t) * pfnnum;
    return PDescaddr;
}

// 传入的页号为区域内偏移
phys_addr_t Part_Pfnnum_To_PDescaddr(zone_t *zone, size_t pfnnum)
{
    phys_addr_t PDescaddr = nullptr;
    phys_addr_t Part_pfn_table_addr = (phys_addr_t)zone->pfn_table_ptr;
    PDescaddr = Part_pfn_table_addr + sizeof(page_t) * pfnnum;
    return PDescaddr;
}

size_t Global_PDescaddr_To_PFnnum(mmnode_t *node, page_t *page)
{
    phys_addr_t Global_pfn_table_addr = node->pfn_table_ptr;
    size_t offset = ((size_t)page - Global_pfn_table_addr) / sizeof(page_t);
    return offset;
}

size_t Part_PDescaddr_To_PFnnum(zone_t *zone, page_t *page)
{
    phys_addr_t Part_pfn_table_addr = (phys_addr_t)zone->pfn_table_ptr;
    size_t offset = ((size_t)page - Part_pfn_table_addr) / sizeof(page_t);
    return offset;
}

phys_addr_t PDescaddr_To_Phypage(phys_addr_t node_pfn_base, page_t *page)
{
    phys_addr_t page_table_base_addr = ((mmnode_t *)node_pfn_base)->pfn_table_ptr;
    phys_addr_t page_addr = (phys_addr_t)page;
    size_t offset = (page_addr - page_table_base_addr) / sizeof(page_t);
    phys_addr_t keep = (phys_addr_t)(offset << 12);
    return keep;
}

phys_addr_t Phypage_To_PDescaddr(zone_t *zone, phys_addr_t page)
{
    phys_addr_t offset = (page & (~0xfff)) >> 12;
    phys_addr_t page_table_base_addr = (phys_addr_t)zone->pfn_table_ptr;
    phys_addr_t PDescaddr = page_table_base_addr + offset * sizeof(page_t);
    return PDescaddr;
}

void mmnode_pfn_init()
{
    if (bootmem_data.numa_count == 1)
    {
        size_t keep = bootmem_data.current_bump_ptr;
        zone_t *zone = 0;
        first_node->low_end_zone = (phys_addr_t)first_node + sizeof(mmnode_t);
        zone = (zone_t *)first_node->low_end_zone;
        zone->type = ZONE_TYPE_LOW_END;
        zone->pfn_table_ptr = nullptr;
        first_node->dma_zone = first_node->low_end_zone + sizeof(zone_t);
        zone = (zone_t *)first_node->dma_zone;
        zone->type = ZONE_TYPE_DMA;
        zone->pfn_table_ptr = nullptr;
        first_node->dma32_zone = first_node->dma_zone + sizeof(zone_t);
        zone = (zone_t *)first_node->dma32_zone;
        zone->type = ZONE_TYPE_DMA32;
        zone->pfn_table_ptr = nullptr;
        first_node->normal_zone = first_node->dma32_zone + sizeof(zone_t);
        zone = (zone_t *)first_node->normal_zone;
        zone->type = ZONE_TYPE_NORMAL;
        zone->pfn_table_ptr = nullptr;
        first_node->pfn_table_ptr = (phys_addr_t)keep;
        page_t *page_ptr = (page_t *)keep;
        for (size_t i = 0; i < first_node->pfn_count; i++, page_ptr++)
        {
            page_ptr->head = 0;
            page_ptr->order = 0;
            page_ptr->size = 4096;
            page_ptr->type = PAGE_TYPE_FREE;
            page_ptr->status = PAGE_STATUS_FREE;
            page_ptr->ref_count = 0;
            page_ptr->mmap_count = 0;
            page_ptr->free_ptr.prev = nullptr;
            page_ptr->free_ptr.next = nullptr;
        }
        keep = (size_t)page_ptr;
        keep = (keep + 4095) & (~4095);
        bootmem_data.current_bump_ptr = keep;
    }
    else
    {
    }
}

void mmzone_buddy_init(zone_t *zone)
{
    for (size_t i = 0; i < MAX_ORDER; i++)
    {
        zone->free_list[i].free_count = 0;
        zone->free_list[i].order = i;
        zone->free_list[i].size = (1 << i) << 12;
        zone->free_list[i].next = nullptr;
    }
    // 直接用逻辑页号来直接偏移struct page 最后逻辑页号转换成为struct page的物理地址
    // 伙伴系统的初始化使用尾插法
    size_t pfn_keep = 0;
    size_t pfn_count = zone->pfn_count;
    size_t order = MAX_ORDER - 1;
    page_t *rear = nullptr;
    while (pfn_count > 0)
    {
        if (pfn_count >= (1 << order))
        {
            zone->free_list[order].free_count++;
            page_t *keep_page = (page_t *)Part_Pfnnum_To_PDescaddr(zone, pfn_keep);
            keep_page->head = 1;
            keep_page->order = order;
            keep_page->size = zone->free_list[order].size;
            if (!zone->free_list[order].next)
                zone->free_list[order].next = keep_page;
            else
            {
                rear->free_ptr.next = keep_page;
                keep_page->free_ptr.prev = rear;
            }
            rear = keep_page;
            pfn_keep += (1 << order);
            pfn_count -= (1 << order);
        }
        else
        {
            order--;
            rear = nullptr;
        }
    }
}

void mmzone_init()
{
    if (bootmem_data.numa_count == 1)
    {
        zone_t *zone = (zone_t *)first_node->low_end_zone;
        size_t keep_phy_page = 0;
        for (size_t i = 0; i < ZONE_COUNT; i++, zone++)
        {
            switch (zone->type)
            {
            case ZONE_TYPE_LOW_END:
                zone->phy_start_pfn = keep_phy_page;
                zone->pfn_count = ZONE_TYPE_LOW_END_SIZE;
                zone->pfn_table_ptr = Global_Pfnnum_To_PDescaddr(first_node, keep_phy_page);
                zone->mmnode_addr = (phys_addr_t)first_node;
                mmzone_buddy_init(zone);
                keep_phy_page += zone->pfn_count;
                break;

            case ZONE_TYPE_DMA:
                zone->phy_start_pfn = keep_phy_page;
                zone->pfn_count = ZONE_TYPE_DMA_SIZE;
                zone->pfn_table_ptr = Global_Pfnnum_To_PDescaddr(first_node, keep_phy_page);
                zone->mmnode_addr = (phys_addr_t)first_node;
                mmzone_buddy_init(zone);
                keep_phy_page += zone->pfn_count;
                break;

            case ZONE_TYPE_DMA32:
                zone->phy_start_pfn = keep_phy_page;
                zone->pfn_count = ZONE_TYPE_DMA32_SIZE;
                zone->pfn_table_ptr = Global_Pfnnum_To_PDescaddr(first_node, keep_phy_page);
                zone->mmnode_addr = (phys_addr_t)first_node;
                mmzone_buddy_init(zone);
                keep_phy_page += zone->pfn_count;
                break;

            case ZONE_TYPE_NORMAL:
                zone->phy_start_pfn = keep_phy_page;
                zone->pfn_count = ZONE_TYPE_NORMAL_SIZE;
                zone->pfn_table_ptr = Global_Pfnnum_To_PDescaddr(first_node, keep_phy_page);
                zone->mmnode_addr = (phys_addr_t)first_node;
                mmzone_buddy_init(zone);
                keep_phy_page += zone->pfn_count;
                break;

            default:
                break;
            }
        }
    }
    else
    {
    }
}