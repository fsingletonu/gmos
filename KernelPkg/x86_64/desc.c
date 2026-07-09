#include <x86_64/desc.h>

void reload_gdt(mmnode_t *node, size_t core_count)
{
    x86_64_gdtr_t gdtr_val = {0, 0};

    size_t size = DEFAULT_GDT_ENTRY * sizeof(x86_gdt_desc_t) + core_count * sizeof(x64_gdt_desc_t);
    size_t pages = ((size + 4095) & (~4096)) >> 12;
    size_t order = 0;
    if (pages <= 1)
        order = 0;
    else
    {
        asm volatile("bsr %1, %0" : "=r"(order) : "r"(pages - 1));
        order++;
    }
    page_t *blk = alloc_pages((mmzone_t *)node->mmzone[2], order);
    phys_addr base = page2phys((mmzone_t *)node->mmzone[2], blk);
    setmem((void *)base, pages * PAGE_SIZE, 0);

    x86_gdt_desc_t *entry = (x86_gdt_desc_t *)base;
    x86_gdt_desc_t *code = &entry[1];
    code->limit_low = 0x0000;
    code->base_low = 0x0000;
    code->base_middle = 0x00;
    code->type = 0xa;
    code->seg = 1;
    code->DPL = 0;
    code->present = 1;
    code->high_seg = 0x0;
    code->available = 0;
    code->long_mode = 1;
    code->protected_or_real = 0;
    code->granularity = 0;
    code->base_high = 0x00;
    x86_gdt_desc_t *data = &entry[2];
    data->limit_low = 0x0000;
    data->base_low = 0x0000;
    data->base_middle = 0x00;
    data->type = 0x2;
    data->seg = 1;
    data->DPL = 0;
    data->present = 1;
    data->high_seg = 0x0;
    data->available = 0;
    data->long_mode = 0;
    data->protected_or_real = 0;
    data->granularity = 0;
    data->base_high = 0x00;

    gdtr_val.limit = size - 1;
    gdtr_val.base.x64_base = base;

    uint64_t next_addr = (uint64_t)&&next_code;

    asm volatile(
        "pushq %[ss]\n\t"
        "pushq %%rsp\n\t"
        "addq $8, (%%rsp)\n\t"
        "pushq %[rflags]\n\t"
        "pushq %[cs]\n\t"
        "pushq %[rip]\n\t"
        :
        : [ss] "i"(0x10),
          [cs] "i"(0x08),
          [rflags] "i"(0x202),
          [rip] "r"(next_addr)
        : "memory");

    asm volatile("lgdt %0" ::"m"(gdtr_val) : "memory");
    asm volatile("iretq" ::: "memory");

next_code:
    asm volatile(
        "mov %[ds], %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        :
        : [ds] "i"(0x10)
        : "ax", "memory");
}