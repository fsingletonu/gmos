#ifndef _IDT_H_
#define _IDT_H_

#include <datatype.h>
#include <mm/zone.h>
#include <lib/string.h>
#include <x86_64/asm.h>
#include <x86_64/desc.h>

typedef enum
{
    GATE_TYPE_INTERRUPT = 0b1110,
    GATE_TYPE_TRAP = 0b1111
} IDT_GATE_TYPE;

struct interrupt_frame
{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

typedef void (*send_eoi_fn)(size_t irq);

void idt_init(mmzone_t *zone, void **idt_ptr);
void idt_register_eoi(send_eoi_fn fn);
void idt_register_interrupt_trap(void *idt_ptr, uint8_t num, phys_addr fn_addr, IDT_GATE_TYPE type);
void send_eoi(size_t irq);

#endif
