#ifndef _IOX64_H
#define _IOX64_H

/*
 * 64位遵循 System V AMD64 ABI，传参顺序为 rdi, rsi, rdx, rcx, r8, r9，其余压栈
 * 本函数为叶子函数，不调用其他函数，不破坏被调用者保存寄存器，无需建立栈帧
 * 返回值：8位->al，16位->ax，32位->eax，64位->rax
 */
#include <datatype.h>

uint8_t in_byte_x64(uint16_t port);
uint16_t in_word_x64(uint16_t port);
uint32_t in_dword_x64(uint16_t port);

void out_byte_x64(uint16_t port, uint8_t data);
void out_word_x64(uint16_t port, uint16_t data);
void out_dword_x64(uint16_t port, uint32_t data);

#endif
