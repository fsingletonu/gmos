#include <x86_64/asm.h>

// 这个需要长跳jmp dword
void set_cs(uint16_t selector)
{
}

void set_ds(uint16_t selector)
{
    asm volatile("mov %0, %%ds"
                 :
                 : "m"(selector)
                 :);
}

void set_ss(uint16_t selector)
{
    asm volatile("mov %0, %%ss"
                 :
                 : "m"(selector)
                 :);
}

void set_es(uint16_t selector)
{
    asm volatile("mov %0, %%es"
                 :
                 : "m"(selector)
                 :);
}

void set_fs(uint16_t selector)
{
    asm volatile("mov %0, %%fs"
                 :
                 : "m"(selector)
                 :);
}

void set_gs(uint16_t selector)
{
    asm volatile("mov %0, %%gs"
                 :
                 : "m"(selector)
                 :);
}

uint16_t get_cs()
{
    uint16_t cs = 0;
    asm volatile("mov %%cs, %0"
                 : "=r"(cs)
                 :
                 : "memory");
    return cs;
}

uint16_t get_ds()
{
    uint16_t ds = 0;
    asm volatile("mov %%ds, %0"
                 : "=r"(ds)
                 :
                 : "memory");
    return ds;
}

uint16_t get_ss()
{
    uint16_t ss = 0;
    asm volatile("mov %%ss, %0"
                 : "=r"(ss)
                 :
                 : "memory");
    return ss;
}

uint16_t get_es()
{
    uint16_t es = 0;
    asm volatile("mov %%es, %0"
                 : "=r"(es)
                 :
                 : "memory");
    return es;
}

uint16_t get_fs()
{
    uint16_t fs = 0;
    asm volatile("mov %%fs, %0"
                 : "=r"(fs)
                 :
                 : "memory");
    return fs;
}

uint16_t get_gs()
{
    uint16_t gs = 0;
    asm volatile("mov %%gs, %0"
                 : "=r"(gs)
                 :
                 : "memory");
    return gs;
}

void lidt(void *idtr_addr)
{
    asm volatile("lidt (%0)"
                 :
                 : "r"(idtr_addr)
                 : "memory");
}

void cli()
{
    asm volatile("cli");
}

void sti()
{
    asm volatile("sti");
}