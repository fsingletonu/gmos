#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <datatype.h>
#include <drivers/chr_drv/iox64.h>
#include <init/idt.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_COMMAND_PORT 0x64

// 大小写都差32
extern const uint8_t chr_set1_ascii[256];
extern const uint8_t chr_set2_ascii[256];
extern const uint8_t chr_set3_ascii[256];

void switch_set2();
__attribute__((interrupt)) void keyboard_handler(struct interrupt_frame *frame);

#endif
