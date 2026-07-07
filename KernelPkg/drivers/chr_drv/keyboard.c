#ifndef _PS2KEYBOARD_H_
#define _PS2KEYBOARD_H_

#include <datatype.h>
#include <drivers/chr_drv/keyboard.h>

const uint8_t chr_set1_ascii[256] = {
    [0x01] = 0x1b, // ESC
    [0x02] = '1',  // 1 !
    [0x03] = '2',  // 2 @
    [0x04] = '3',  // 3 #
    [0x05] = '4',  // 4 $
    [0x06] = '5',  // 5 %
    [0x07] = '6',  // 6 ^
    [0x08] = '7',  // 7 &
    [0x09] = '8',  // 8 *
    [0x0a] = '9',  // 9 (
    [0x0b] = '0',  // 0 )
    [0x0c] = '-',  // - _
    [0x0d] = '=',  // = +
    [0x0e] = 0x08, // Backspace
    [0x0f] = 0x09, // Tab
    [0x10] = 'q',  // q Q
    [0x11] = 'w',  // w W
    [0x12] = 'e',  // e E
    [0x13] = 'r',  // r R
    [0x14] = 't',  // t T
    [0x15] = 'y',  // y Y
    [0x16] = 'u',  // u U
    [0x17] = 'i',  // i I
    [0x18] = 'o',  // o O
    [0x19] = 'p',  // p P
    [0x1a] = '[',  // [ {
    [0x1b] = ']',  // ] }
    [0x1c] = 0x0d, // Enter
    [0x1d] = 0xff, // Left Ctrl
    [0x1e] = 'a',  // a A
    [0x1f] = 's',  // s S
    [0x20] = 'd',  // d D
    [0x21] = 'f',  // f F
    [0x22] = 'g',  // g G
    [0x23] = 'h',  // h H
    [0x24] = 'j',  // j J
    [0x25] = 'k',  // k K
    [0x26] = 'l',  // l L
    [0x27] = ';',  // ; :
    [0x28] = 0x27, // ' "
    [0x29] = '`',  // ` ~
    [0x2a] = 0xff, // Left Shift
    [0x2b] = '\\', // \ |
    [0x2c] = 'z',  // z Z
    [0x2d] = 'x',  // x X
    [0x2e] = 'c',  // c C
    [0x2f] = 'v',  // v V
    [0x30] = 'b',  // b B
    [0x31] = 'n',  // n N
    [0x32] = 'm',  // m M
    [0x33] = ',',  // , <
    [0x34] = '.',  // . >
    [0x35] = '/',  // / ?
    [0x36] = 0xff, // Right Shift
    [0x37] = '*',  // * (小键盘)
    [0x38] = 0xff, // Left Alt
    [0x39] = ' ',  // Space
    [0x3a] = 0xff, // Caps Lock
    [0x3b] = 0xff, // F1
    [0x3c] = 0xff, // F2
    [0x3d] = 0xff, // F3
    [0x3e] = 0xff, // F4
    [0x3f] = 0xff, // F5
    [0x40] = 0xff, // F6
    [0x41] = 0xff, // F7
    [0x42] = 0xff, // F8
    [0x43] = 0xff, // F9
    [0x44] = 0xff, // F10
    [0x45] = 0xff, // Num Lock
    [0x46] = 0xff, // Scroll Lock
    [0x47] = '7',  // Home / 7
    [0x48] = '8',  // ↑ / 8
    [0x49] = '9',  // Page Up / 9
    [0x4a] = '-',  // - (小键盘)
    [0x4b] = '4',  // ← / 4
    [0x4c] = '5',  // 5
    [0x4d] = '6',  // → / 6
    [0x4e] = '+',  // + (小键盘)
    [0x4f] = '1',  // End / 1
    [0x50] = '2',  // ↓ / 2
    [0x51] = '3',  // Page Down / 3
    [0x52] = '0',  // Ins / 0
    [0x53] = '.',  // Del / .
    [0x57] = 0xff, // F11
    [0x58] = 0xff, // F12
    [0xe0] = 0xff, // E0前缀 (Print Screen / Pause等扩展键)
};

const uint8_t chr_set2_ascii[256] = {
    [0x01] = 0xff, // F9
    [0x03] = 0xff, // F5
    [0x04] = 0xff, // F3
    [0x05] = 0xff, // F1
    [0x06] = 0xff, // F2
    [0x07] = 0xff, // F12
    [0x09] = 0xff, // F10
    [0x0a] = 0xff, // F8
    [0x0b] = 0xff, // F6
    [0x0c] = 0xff, // F4
    [0x0d] = 0x09, // Tab
    [0x0e] = '`',  // ` ~
    [0x11] = 0xff, // Left Alt
    [0x12] = 0xff, // Left Shift
    [0x14] = 0xff, // Left Ctrl
    [0x15] = 'q',  // q Q
    [0x16] = '1',  // 1 !
    [0x1a] = 'z',  // z Z
    [0x1b] = 's',  // s S
    [0x1c] = 'a',  // a A
    [0x1d] = 'w',  // w W
    [0x1e] = '2',  // 2 @
    [0x21] = 'c',  // c C
    [0x22] = 'x',  // x X
    [0x23] = 'd',  // d D
    [0x24] = 'e',  // e E
    [0x25] = '4',  // 4 $
    [0x26] = '3',  // 3 #
    [0x29] = 0x20, // Space
    [0x2a] = 'v',  // v V
    [0x2b] = 'f',  // f F
    [0x2c] = 't',  // t T
    [0x2d] = 'r',  // r R
    [0x2e] = '5',  // 5 %
    [0x31] = 'n',  // n N
    [0x32] = 'b',  // b B
    [0x33] = 'h',  // h H
    [0x34] = 'g',  // g G
    [0x35] = 'y',  // y Y
    [0x36] = '6',  // 6 ^
    [0x3a] = 'm',  // m M
    [0x3b] = 'j',  // j J
    [0x3c] = 'u',  // u U
    [0x3d] = '7',  // 7 &
    [0x3e] = '8',  // 8 *
    [0x41] = ',',  // , <
    [0x42] = 'k',  // k K
    [0x43] = 'i',  // i I
    [0x44] = 'o',  // o O
    [0x45] = '0',  // 0 )
    [0x46] = '9',  // 9 (
    [0x49] = '.',  // . >
    [0x4a] = '/',  // / ?
    [0x4b] = 'l',  // l L
    [0x4c] = ';',  // ; :
    [0x4d] = 'p',  // p P
    [0x4e] = '-',  // - _
    [0x52] = 0x27, // ' "
    [0x54] = '[',  // [ {
    [0x55] = '=',  // = +
    [0x58] = 0xff, // Caps Lock
    [0x59] = 0xff, // Right Shift
    [0x5a] = 0x0d, // Enter
    [0x5b] = ']',  // ] }
    [0x5d] = '\\', // \ |
    [0x66] = 0x08, // Backspace
    [0x69] = '1',  // End / 1
    [0x6b] = '4',  // ← / 4
    [0x6c] = '7',  // Home / 7
    [0x70] = '0',  // Ins / 0
    [0x71] = '.',  // Del / .
    [0x72] = '2',  // ↓ / 2
    [0x73] = '5',  // 5
    [0x74] = '6',  // → / 6
    [0x75] = '8',  // ↑ / 8
    [0x76] = 0x1b, // ESC
    [0x77] = 0xff, // Num Lock
    [0x78] = 0xff, // F11
    [0x79] = '+',  // +
    [0x7a] = '3',  // Page Down / 3
    [0x7b] = '-',  // -
    [0x7c] = '*',  // *
    [0x7d] = '9',  // Page Up / 9
    [0x83] = 0xff, // F7
    [0xe0] = 0xff, // E0扩展键前缀
    [0xe1] = 0xff, // E1扩展键前缀 (Pause)
    [0xf0] = 0xff, // 断码前缀
};

const uint8_t chr_set3_ascii[256] = {
    [0x01] = 0xff, // 未知键
    [0x02] = '1',  // 1 !
    [0x03] = '2',  // 2 @
    [0x04] = '3',  // 3 #
    [0x05] = '4',  // 4 $
    [0x06] = '5',  // 5 %
    [0x07] = '6',  // 6 ^
    [0x08] = 0x1b, // ESC
    [0x09] = '7',  // 7 &
    [0x0a] = '8',  // 8 *
    [0x0b] = '9',  // 9 (
    [0x0c] = '0',  // 0 )
    [0x0d] = '-',  // - _
    [0x0e] = '=',  // = +
    [0x0f] = 0x08, // Backspace
    [0x10] = 0x09, // Tab
    [0x11] = 'q',  // q Q
    [0x12] = 'w',  // w W
    [0x13] = 'e',  // e E
    [0x14] = 'r',  // r R
    [0x15] = 't',  // t T
    [0x16] = 'y',  // y Y
    [0x17] = 'u',  // u U
    [0x18] = 'i',  // i I
    [0x19] = 'o',  // o O
    [0x1a] = 'p',  // p P
    [0x1b] = '[',  // [ {
    [0x1c] = ']',  // ] }
    [0x1d] = 0x0d, // Enter
    [0x1e] = 0xff, // Left Ctrl
    [0x1f] = 'a',  // a A
    [0x20] = 's',  // s S
    [0x21] = 'd',  // d D
    [0x22] = 'f',  // f F
    [0x23] = 'g',  // g G
    [0x24] = 'h',  // h H
    [0x25] = 'j',  // j J
    [0x26] = 'k',  // k K
    [0x27] = 'l',  // l L
    [0x28] = ';',  // ; :
    [0x29] = 0x27, // ' "
    [0x2a] = '`',  // ` ~
    [0x2b] = 0xff, // Left Shift
    [0x2c] = '\\', // \ |
    [0x2d] = 'z',  // z Z
    [0x2e] = 'x',  // x X
    [0x2f] = 'c',  // c C
    [0x30] = 'v',  // v V
    [0x31] = 'b',  // b B
    [0x32] = 'n',  // n N
    [0x33] = 'm',  // m M
    [0x34] = ',',  // , <
    [0x35] = '.',  // . >
    [0x36] = '/',  // / ?
    [0x37] = 0xff, // Right Shift
    [0x38] = 0xff, // Left Alt
    [0x39] = ' ',  // Space
    [0x3a] = 0xff, // Caps Lock
    [0x3b] = 0xff, // F1
    [0x3c] = 0xff, // F2
    [0x3d] = 0xff, // F3
    [0x3e] = 0xff, // F4
    [0x3f] = 0xff, // F5
    [0x40] = 0xff, // F6
    [0x41] = 0xff, // F7
    [0x42] = 0xff, // F8
    [0x43] = 0xff, // F9
    [0x44] = 0xff, // F10
    [0x45] = 0xff, // Num Lock
    [0x46] = 0xff, // Scroll Lock
    [0x47] = '7',  // Home / 7
    [0x48] = '8',  // ↑ / 8
    [0x49] = '9',  // Page Up / 9
    [0x4a] = '-',  // - (小键盘)
    [0x4b] = '4',  // ← / 4
    [0x4c] = '5',  // 5
    [0x4d] = '6',  // → / 6
    [0x4e] = '+',  // + (小键盘)
    [0x4f] = '1',  // End / 1
    [0x50] = '2',  // ↓ / 2
    [0x51] = '3',  // Page Down / 3
    [0x52] = '0',  // Ins / 0
    [0x53] = '.',  // Del / .
    [0x54] = 0xff, // 未知键
    [0x55] = '*',  // * (小键盘)
    [0x56] = 0xff, // F11
    [0x57] = 0xff, // F12
    [0x58] = 0xff, // Left GUI
    [0x59] = 0xff, // Right GUI
    [0x5a] = 0xff, // App (Menu键)
    [0x5b] = 0xff, // Right Alt
    [0x5c] = 0xff, // Right Ctrl
    [0x5d] = 0xff, // 未知键
    [0x5e] = 0xff, // 未知键
    [0x5f] = 0xff, // 未知键
    [0x60] = 0xff, // 未知键
    [0x61] = 0xff, // 未知键
    [0x62] = 0xff, // 未知键
    [0x63] = 0xff, // 未知键
    [0x64] = 0xff, // 未知键
    [0x65] = 0xff, // 未知键
    [0x66] = 0x08, // Backspace (重复)
    [0x67] = 0xff, // 未知键
    [0x68] = 0xff, // 未知键
    [0x69] = '1',  // End / 1 (重复)
    [0x6a] = 0xff, // 未知键
    [0x6b] = '4',  // ← / 4 (重复)
    [0x6c] = '7',  // Home / 7 (重复)
    [0x6d] = 0xff, // 未知键
    [0x6e] = 0xff, // 未知键
    [0x6f] = 0xff, // 未知键
    [0x70] = '0',  // Ins / 0 (重复)
    [0x71] = '.',  // Del / . (重复)
    [0x72] = '2',  // ↓ / 2 (重复)
    [0x73] = '5',  // 5 (重复)
    [0x74] = '6',  // → / 6 (重复)
    [0x75] = '8',  // ↑ / 8 (重复)
    [0x76] = 0xff, // 未知键
    [0x77] = 0xff, // 未知键
    [0x78] = 0xff, // 未知键
    [0x79] = '+',  // + (重复)
    [0x7a] = '3',  // Page Down / 3 (重复)
    [0x7b] = '-',  // - (重复)
    [0x7c] = '*',  // * (重复)
    [0x7d] = '9',  // Page Up / 9 (重复)
    [0x7e] = 0xff, // 未知键
    [0x7f] = 0xff, // 未知键
    [0x84] = 0xff, // 打印/系统请求
    [0x85] = 0xff, // 未知键
    [0x86] = 0xff, // 未知键
    [0x87] = 0xff, // 未知键
};

void wait_output_full()
{
    while (!(in_byte_x64(KEYBOARD_STATUS_COMMAND_PORT) & 0x01))
        ;
}

void wait_input_empty()
{
    while (in_byte_x64(KEYBOARD_STATUS_COMMAND_PORT) & 0x02)
        ;
}

void wait_ack()
{
    uint8_t data;
    do
    {
        while (!(in_byte_x64(0x64) & 0x01))
            ;
        data = in_byte_x64(0x60);
    } while (data != 0xFA);
}

void switch_set2()
{
    wait_input_empty();
    out_byte_x64(KEYBOARD_STATUS_COMMAND_PORT, 0xD4);
    wait_input_empty();
    out_byte_x64(KEYBOARD_DATA_PORT, 0xf0);
    wait_ack();
    wait_input_empty();
    out_byte_x64(KEYBOARD_STATUS_COMMAND_PORT, 0xD4);
    wait_input_empty();
    out_byte_x64(KEYBOARD_DATA_PORT, 0x02);
    wait_ack();
}

__attribute__((interrupt)) void keyboard_handler(struct interrupt_frame *frame)
{
#if defined(__i386__)

#elif defined(__x86_64__)
    uint8_t status_command = in_byte_x64(KEYBOARD_STATUS_COMMAND_PORT);
    uint8_t scan_code = in_byte_x64(KEYBOARD_DATA_PORT);
    uint8_t data = chr_set1_ascii[scan_code];
    DEBUG;
    send_eoi(0x21);
#endif
}

#endif
