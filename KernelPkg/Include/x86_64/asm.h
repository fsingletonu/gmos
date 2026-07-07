#ifndef _ASM_H_
#define _ASM_H_

#include <datatype.h>

void set_cs(uint16_t selector);
void set_ds(uint16_t selector);
void set_ss(uint16_t selector);
void set_es(uint16_t selector);
void set_fs(uint16_t selector);
void set_gs(uint16_t selector);

uint16_t get_cs();
uint16_t get_ds();
uint16_t get_ss();
uint16_t get_es();
uint16_t get_fs();
uint16_t get_gs();

void lidt(void *idtr_addr);

void cli();
void sti();

#endif
