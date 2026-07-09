#include <mm/early_alloc.h>

void insert_memblock(EFI_MEMORY_DESCRIPTOR *desc, size_t used_pages)
{
    size_t use_size = memory_new_use_memblock->use_size;
    size_t all_size = memory_new_use_memblock->all_size;
    size_t size_per_desc = sizeof(memblock_entry_t);
    if (use_size < all_size && use_size >= size_per_desc)
    {
        memblock_entry_t *entry = (memblock_entry_t *)memory_new_use_memblock->stack_top;
        entry->PhysicalStart = desc->PhysicalStart;
        entry->NumberOfPages = desc->NumberOfPages;
        entry->used_pages += used_pages;
        memory_new_use_memblock->use_size += sizeof(memblock_entry_t);
        memory_new_use_memblock->stack_top += sizeof(memblock_entry_t);
        memory_new_use_memblock->desc_count++;
        DEBUG;
    }
}

void memblock_init(BootInfoHead *Info)
{
    size_t pages = INIT_MEMBLOCK_REGIONS_PAGES;
    size_t entry_number = Info->MemoryInfo.MemorySize / Info->MemoryInfo.DescriptorSize;
    for (size_t i = 0; i < entry_number; i++)
    {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)Info->MemoryInfo.BaseAddr + i * Info->MemoryInfo.DescriptorSize);
        if (desc->Type != EfiConventionalMemory)
            continue;
        if (desc->NumberOfPages >= pages)
        {
            setmem((void *)desc->PhysicalStart, INIT_MEMBLOCK_REGIONS_PAGES * 4096, 0);
            memory_new_use_memblock = (memblock_head_t *)desc->PhysicalStart;
            memory_new_use_memblock->all_size = pages * PAGE_SIZE;
            memory_new_use_memblock->use_size = PAGE_SIZE;
            memory_new_use_memblock->stack_end = desc->PhysicalStart + PAGE_SIZE;
            memory_new_use_memblock->stack_top = desc->PhysicalStart + PAGE_SIZE;
            insert_memblock(desc, INIT_MEMBLOCK_REGIONS_PAGES);
            break;
        }
    }
}

phys_addr early_alloc_pages(BootInfoHead *Info, uint64_t pages)
{
    // 先看是否memblock有可用的块；如果有的话 直接计算返回；如果没有 需要再去MemoryMap中去顺序取下一个
    for (size_t i = 0; i < memory_new_use_memblock->desc_count; i++)
    {
        memblock_entry_t *entry = (memblock_entry_t *)((uint8_t *)memory_new_use_memblock->stack_end + i * sizeof(memblock_entry_t));
        if (entry->used_pages + pages <= entry->NumberOfPages)
        {
            phys_addr addr = entry->PhysicalStart + PAGE_SIZE * entry->used_pages;
            entry->used_pages += pages;
            return addr;
        }
    }
    // 到这里已经说明没有足够的早期记载的消息了 要用O(n^2)的算法去遍历MemoryMap和memblock
    size_t entry_number = Info->MemoryInfo.MemorySize / Info->MemoryInfo.DescriptorSize;
    bool found = FALSE;
    for (size_t i = 0; i < entry_number; i++)
    {
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)Info->MemoryInfo.BaseAddr + i * Info->MemoryInfo.DescriptorSize);
        if (desc->Type != EfiConventionalMemory)
            continue;
        // 这里判断的是和memblock中的东西是否有重复
        for (size_t j = 0; j < memory_new_use_memblock->desc_count; j++)
        {
            memblock_entry_t *entry = (memblock_entry_t *)((uint8_t *)memory_new_use_memblock->stack_end + j * sizeof(memblock_entry_t));
            if (desc->PhysicalStart == entry->PhysicalStart)
                continue;
            if (desc->NumberOfPages >= pages)
            {
                insert_memblock(desc, 0);
                found = TRUE;
                break;
            }
        }
        if (found)
        {
            return early_alloc_pages(Info, pages);
        }
    }
}

void struct_page_init(BootInfoHead *Info)
{
    size_t mem_map_pages = (Info->AllOfPages * sizeof(page_t) + 4095) / 4096;
    phys_addr mem_map = early_alloc_pages(Info, mem_map_pages);
    memblock_entry_t *test = (memblock_entry_t *)(0x1018);
    g_first_node->mem_map = mem_map;
    setmem((void *)mem_map, mem_map_pages * PAGE_SIZE, 0);
    for (size_t i = 0; i < Info->AllOfPages; i++)
    {
        page_t *page = (page_t *)((uint8_t *)mem_map + i * sizeof(page_t));
        page->flags = FLAGS_RESERVED;
    }
    /*
     * 第一次填充（根据MemoryMap）（来源有两种：1、UEFI 2、BIOS 0xE820）
     * 这里只回收以下几种：
     * 1、EfiLoaderCode
     * 2、EfiLoaderData
     * 3、EfiBootServicesCode
     * 4、EfiBootServicesData
     * 7、EfiConventionalMemory
     */
    size_t MemorySize = Info->MemoryInfo.MemorySize;
    size_t DescSize = Info->MemoryInfo.DescriptorSize;
    size_t DescNum = MemorySize / DescSize;
    for (size_t i = 0; i < DescNum; i++)
    {
        EFI_MEMORY_DESCRIPTOR *entry = (EFI_MEMORY_DESCRIPTOR *)((uint8_t *)Info->MemoryInfo.BaseAddr + i * DescSize);
        switch (entry->Type)
        {
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            page_t *page = (page_t *)phys2page(g_first_node, entry->PhysicalStart);
            for (size_t j = 0; j < entry->NumberOfPages; j++, page++)
            {
                page->flags = FLAGS_FREE;
            }
            break;

        default:
            break;
        }
    }

    /*
     * 第二次填充（根据Mem_Block）
     */
    for (size_t i = 0; i < memory_new_use_memblock->desc_count; i++)
    {
        memblock_entry_t *entry = (memblock_entry_t *)((uint8_t *)memory_new_use_memblock->stack_end + i * sizeof(memblock_entry_t));
        page_t *page = (page_t *)phys2page(g_first_node, entry->PhysicalStart);
        for (size_t j = 0; j < entry->used_pages; j++, page++)
        {
            page->flags = FLAGS_RESERVED;
        }
    }
}

void node_zone_init(BootInfoHead *Info)
{
    size_t pages = (MM_SIZE_PRE_UMA + 4095) / 4096;
    phys_addr node_addr = early_alloc_pages(Info, pages);
    setmem((void *)node_addr, pages * PAGE_SIZE, 0);
    g_first_node = (mmnode_t *)node_addr;
    for (size_t i = 0; i < ZONE_PRE_NODE; i++)
    {
        g_first_node->mmzone[i] = (phys_addr)g_first_node + sizeof(mmnode_t) + i * sizeof(mmzone_t);
    }
    g_first_node->pf_number = Info->AllOfPages;
}

void mm_init(BootInfoHead *Info)
{
    node_zone_init(Info);
    struct_page_init(Info);
}
