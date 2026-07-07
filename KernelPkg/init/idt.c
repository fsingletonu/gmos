#include <init/idt.h>

static send_eoi_fn eoi_handler = NULL;

void send_eoi(size_t irq);

__attribute__((interrupt)) void isr_default_handler(struct interrupt_frame *frame)
{
    // 这个中断号应该动态获取
    send_eoi(0x21);
}

#if defined(__i386__)

void fill_x86_idt(void *base, uint16_t cs)
{
    x86_idt_desc_t *entry = (x86_idt_desc_t *)base;
    uint32_t default_addr = (uint32_t)&isr_default_handler;
    for (size_t i = 0; i < INT_NUM; i++, entry++)
    {
        entry->low_offset = (uint16_t)(default_addr & 0xffff);
        entry->selector = cs;
        entry->reserved = 0;
        entry->TYPE = GATE_TYPE_INTERRUPT;
        entry->S = 0;
        entry->DPL = 0;
        entry->P = 1;
        entry->high_offset = (uint16_t)((default_addr >> 16) & 0xffff);
    }
}

#elif defined(__x86_64__)

void fill_x64_idt(void *base, uint16_t cs)
{
    x64_idt_desc_t *entry = (x64_idt_desc_t *)base;
    uint64_t default_addr = (uint64_t)&isr_default_handler;
    for (size_t i = 0; i < INT_NUM; i++, entry++)
    {
        entry->low_offset = (uint16_t)(default_addr & 0xffff);
        entry->selector = cs;
        entry->ist = 0;
        entry->TYPE = GATE_TYPE_INTERRUPT;
        entry->S = 0;
        entry->DPL = 0;
        entry->P = 0;
        entry->mid_offset = (uint16_t)((default_addr >> 16) & 0xffff);
        entry->high_offset = (uint32_t)(default_addr >> 32);
        entry->reserved = 0;
    }
}

#endif

void idt_init(mmzone_t *zone, void **idt_ptr)
{
    x86_64_idtr_t idtr = {0, 0};
    uint16_t cs = get_cs();
    page_t *idt_base_blk = alloc_pages(zone, 0);
    void *idt_base = (void *)page2phys(zone, idt_base_blk);
    *idt_ptr = idt_base;
    if (idt_base == NULL)
        return;
    setmem((void *)idt_base, PAGE_SIZE, 0);

#if defined(__i386__)
    fill_x86_idt(idt_base, cs);
    idtr.limit = sizeof(x86_idt_desc_t) * INT_NUM - 1;
    idtr.base.x86_base.base = (size_t)idt_base;
#elif defined(__x86_64__)
    fill_x64_idt(idt_base, cs);
    idtr.limit = sizeof(x64_idt_desc_t) * INT_NUM - 1;
    idtr.base.x64_base = (size_t)idt_base;
#endif

    lidt((void *)&idtr);
}

void idt_register_eoi(send_eoi_fn fn)
{
    eoi_handler = fn;
}

void send_eoi(size_t irq)
{
    if (eoi_handler)
        eoi_handler(irq);
}

void idt_register_interrupt_trap(void *idt_ptr, uint8_t num, phys_addr fn_addr, IDT_GATE_TYPE type)
{
#if defined(__i386__)
    x86_idt_desc_t *idt_base = (x86_idt_desc_t *)idt_ptr;
    idt_base[num].low_offset = (uint16_t)(fn_addr & 0xffff);
    idt_base[num].TYPE = type;
    idt_base[num].high_offset = (uint16_t)(fn_addr >> 16);

#elif defined(__x86_64__)
    x64_idt_desc_t *idt_base = (x64_idt_desc_t *)idt_ptr;
    idt_base[num].low_offset = (uint16_t)((size_t)fn_addr & 0xffff);
    idt_base[num].TYPE = type;
    idt_base[num].P = 1;
    idt_base[num].mid_offset = (uint16_t)(((size_t)fn_addr >> 16) & 0xffff);
    idt_base[num].high_offset = (uint32_t)(((size_t)fn_addr >> 32));

    x86_64_idtr_t sidt_buf;
    __asm__ volatile("sidt %0" : "=m"(sidt_buf));

    x64_idt_desc_t *entry = (x64_idt_desc_t *)(sidt_buf.base.x64_base);
    x64_idt_desc_t *b = (x64_idt_desc_t *)&entry[0x21];
#endif
}