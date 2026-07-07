#ifndef _DESC_H_
#define _DESC_H_

#include <datatype.h>
#include <lib/string.h>
#include <mm/zone.h>

#define INT_NUM 256
#define DEFAULT_GDT_ENTRY 3

#pragma pack(push, 1)
typedef struct x86_64_gdtr
{
    uint16_t limit;
    union
    {
        struct
        {
            uint32_t base;
            uint32_t reserved;
        } x86_base;
        uint64_t x64_base;
    } base;
} x86_64_gdtr_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct x86_gdt_desc
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t type : 4;
    uint8_t seg : 1;
    uint8_t DPL : 2;
    uint8_t present : 1;
    uint8_t high_seg : 4;
    uint8_t available : 1;
    uint8_t long_mode : 1;
    uint8_t protected_or_real : 1;
    uint8_t granularity : 1;
    uint8_t base_high;
} x86_gdt_desc_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct x64_gdt_desc
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t type : 4; // 段类型 [3:0] 代码段: 1010 (0xA) 数据段: 0010 (0x2) TSS可用: 1001 (0x9) TSS忙:   1011 (0xB)
    uint8_t S : 1;    // 描述符类型 [4] 0 = 系统段 (TSS, LDT) 1 = 代码/数据段
    uint8_t DPL : 2;
    uint8_t P : 1;
    uint8_t limit_high : 4;
    uint8_t AVL : 1; // 系统软件可用位
    uint8_t L : 1;   // 64位代码段标志
    uint8_t D_B : 1; // 默认操作大小 (0=16位, 1=32位)
    uint8_t G : 1;   // 粒度 (0=字节, 1=4KB)
    uint8_t base_high;
    uint32_t base_upper; // 基地址 [63:32]
    uint32_t reserved;   // 保留，必须为 0
} x64_gdt_desc_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct far_jmp_target
{
    uint64_t rip_target;
    uint16_t cs_selector;
} far_jmp_target_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct x86_64_idtr
{
    uint16_t limit;
    union
    {
        struct
        {
            uint32_t base;
            uint32_t reserved;
        } x86_base;
        uint64_t x64_base;
    } base;
} x86_64_idtr_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct x86_idt_desc
{
    uint16_t low_offset;
    uint16_t selector;
    uint8_t reserved;
    uint8_t TYPE : 4;
    uint8_t S : 1;
    uint8_t DPL : 2;
    uint8_t P : 1;
    uint16_t high_offset;
} x86_idt_desc_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct x64_idt_desc
{
    uint16_t low_offset; // 偏移 0-15 位
    uint16_t selector;   // 段选择子
    uint8_t ist;         // 中断栈表 (Interrupt Stack Table) 偏移，低3位有效
    uint8_t TYPE : 4;
    uint8_t S : 1;
    uint8_t DPL : 2;
    uint8_t P : 1;
    uint16_t mid_offset;  // 偏移 16-31 位
    uint32_t high_offset; // 偏移 32-63 位
    uint32_t reserved;    //  保留，必须为0
} x64_idt_desc_t;
#pragma pack(pop)

void reload_gdt(mmnode_t *node, size_t core_count);

#endif
