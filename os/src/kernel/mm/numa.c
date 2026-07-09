/*
 * numa.c
 * (C) 2026 Guo Fu
 */

#include <mm/numa.h>

void mmnode_init()
{
    uint32_t keep = bootmem_data.current_bump_ptr;
    keep = (keep + 4095) & (~4095);
    first_node = (mmnode_t *)keep;
    // 这里说明不是NUMA结构，区域zone指针就放在这一页吧
    if (!seek_acpi_header(SRAT))
    {
        bootmem_data.numa_count = 1;
        bootmem_data.current_bump_ptr += sizeof(struct mmnode);
        first_node->mmnode_id = 0;
        first_node->status = N_NORMAL_MEMORY;
        first_node->persent_pages_num = bootmem_data.memory_page;
        first_node->spanned_pages_num = 0;
        first_node->low_end_zone = nullptr;
        first_node->dma_zone = nullptr;
        first_node->dma32_zone = nullptr;
        first_node->normal_zone = nullptr;
        first_node->pfn_table_ptr = nullptr;
        first_node->pfn_count = bootmem_data.memory_page;
        first_node->next = nullptr;
        keep += 4096;
        bootmem_data.current_bump_ptr = keep;
    }
    // 这里就说明是NUMA结构
    else
    {
        acpi_sdt_header_t *ptr = seek_acpi_header(SRAT);
    }
}