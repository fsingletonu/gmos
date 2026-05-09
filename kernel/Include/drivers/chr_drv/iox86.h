#ifndef _IOX86_H_
#define _IOX86_H_

#include <datatype.h>

uint8_t in_byte_x86(uint16_t port);
uint16_t in_word_x86(uint16_t port);
uint32_t in_dword_x86(uint16_t port);

void out_byte_x86(uint16_t port, uint8_t data);
void out_word_x86(uint16_t port, uint16_t data);
void out_dword_x86(uint16_t port, uint32_t data);

#endif
