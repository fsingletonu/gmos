#ifndef _IOX86_H_
#define _IOX86_H_

/*
 * 这个是给传统兼容用的
 * 而且对于32位的代码 参数是压栈处理的（cdecl） 但对于64位是不同处理的
 *
 */
#include <datatype.h>

uint8_t in_byte_x86(uint16_t port);
uint16_t in_word_x86(uint16_t port);
uint32_t in_dword_x86(uint16_t port);

void out_byte_x86(uint16_t port, uint8_t data);
void out_word_x86(uint16_t port, uint16_t data);
void out_dword_x86(uint16_t port, uint32_t data);

#endif
