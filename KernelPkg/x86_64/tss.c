#include <x86_64/tss.h>

void *alloc_local_rsp0(mmnode_t *node, tss_t *tss_entry)
{
    page_t *blk = alloc_pages((mmzone_t *)node->mmzone[2], IST_ORDER);
    phys_addr rsp0_base = page2phys((mmzone_t *)node->mmzone[2], blk);
    setmem((void *)rsp0_base, (1 << IST_ORDER) * PAGE_SIZE, 0);
    return (void *)rsp0_base;
}

void fill_tss_gdt_entry(size_t cpu_logic_id, tss_t *tss_addr)
{
    x86_64_gdtr_t gdtr = {0, 0};
    asm volatile("sgdt %0"
                 : "=m"(gdtr)
                 :
                 : "memory");

    x64_gdt_desc_t *addr = (x64_gdt_desc_t *)(gdtr.base.x64_base + DEFAULT_GDT_ENTRY * sizeof(x86_gdt_desc_t) + cpu_logic_id * sizeof(x64_gdt_desc_t));
    uint64_t tss_base = (uint64_t)tss_addr;
    uint32_t tss_limit = sizeof(tss_t) - 1;
    addr->limit_low = (uint16_t)tss_limit & 0xffff;
    addr->base_low = (uint16_t)tss_base & 0xffff;
    addr->base_mid = (uint8_t)(tss_base >> 16) & 0xff;
    addr->type = 0x9;
    addr->S = 0;
    addr->DPL = 0;
    addr->P = 1;
    addr->limit_high = (uint8_t)(tss_limit >> 16) & 0xf;
    addr->AVL = 0;
    addr->L = 0;
    addr->D_B = 0;
    addr->G = 0;
    addr->base_high = (uint8_t)((tss_base >> 24) & 0xFF);
    addr->base_upper = (uint32_t)(tss_base >> 32) & 0xffffffff;
    addr->reserved = 0x00000000;
}

tss_t *local_tss_init(mmnode_t *node, size_t cpu_logic_id)
{
    page_t *blk = alloc_pages((mmzone_t *)node->mmzone[2], 0);
    phys_addr base = page2phys((mmzone_t *)node->mmzone[2], blk);
    setmem((void *)base, PAGE_SIZE, 0);

    tss_t *tss_addr = (tss_t *)base;

    void *rsp0 = alloc_local_rsp0(node, tss_addr);

    tss_addr->rsp0 = (uint64_t)rsp0 + (1 << IST_ORDER) * PAGE_SIZE;
    tss_addr->iopb_offset = sizeof(tss_t);

    fill_tss_gdt_entry(cpu_logic_id, tss_addr);

    uint16_t tss_sel = DEFAULT_GDT_ENTRY * sizeof(x86_gdt_desc_t) + cpu_logic_id * sizeof(x64_gdt_desc_t);

    asm volatile("ltr %0" : : "r"(tss_sel));

    return tss_addr;
}
