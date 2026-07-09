#include <init/ards.h>

void bootmem_data_init()
{
    uint32_t *ards_buffer_ptr = (uint32_t *)0x7c00;
    uint32_t *memory_page_ptr = (uint32_t *)0x7c04;
    uint32_t *pt_addr_ptr = (uint32_t *)0x7c08;
    uint32_t *kernel_mapping_addr_ptr = (uint32_t *)0x7c0c;
    uint32_t *kernel_load_addr_ptr = (uint32_t *)0x7c10;
    uint32_t *current_bump_ptr = (uint32_t *)0x7c14;
    bootmem_data.ards_buffer = (uint32_t)*ards_buffer_ptr;
    bootmem_data.memory_page = (uint32_t)*memory_page_ptr;
    bootmem_data.pt_addr = (uint32_t)*pt_addr_ptr;
    bootmem_data.kernel_mapping_addr = (uint32_t)*kernel_mapping_addr_ptr;
    bootmem_data.kernel_load_addr = (uint32_t)*kernel_load_addr_ptr;
    bootmem_data.current_bump_ptr = (((uint32_t)*current_bump_ptr) + 4095) & (~4095);
}

void ards_analyse()
{
    
}