#include <init/apic.h>

void apic_init()
{

    idt_register_eoi(apic_eoi);
}

void apic_eoi(size_t irq)
{
}