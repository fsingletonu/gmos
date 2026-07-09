/*
 * memory.c
 * (C) Guo Fu
 * 这里低端内存已经划分 没有必要再去给系统物理区用alloc_pages申请内存
 */
#include <mm/memory.h>

void bump_allocator()
{
    bootmem_data_init();
    mmnode_init();
    mmnode_pfn_init();
    mmzone_init();
    slab_init();
    pci_device_table_init();
}