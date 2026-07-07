#ifndef _PIC_H_
#define _PIC_H_

#include <datatype.h>
#include <drivers/chr_drv/iox86.h>
#include <drivers/chr_drv/iox64.h>
#include <init/idt.h>

typedef enum
{
    HPCP = 0x20,
    HPDP = 0x21,
    SPCP = 0xa0,
    SPDP = 0xa1
} PIC_PORT;

typedef enum
{
    HIMR = 0x21,
    SIMR = 0xa1
} PIC_IMR;

typedef enum
{
    HICW1 = 0x20,
    HICW2 = 0x21,
    HICW3 = 0x21,
    HICW4 = 0x21
} PIC_HOST_ICW_SET;

typedef enum
{
    SICW1 = 0xa0,
    SICW2 = 0xa1,
    SICW3 = 0xa1,
    SICW4 = 0xa1
} PIC_SLAVE_ICW_SET;

#pragma pack(push, 1)
typedef union ICW1
{
    struct
    {
        uint8_t ic4_enable : 1; // 是不是需要给ICW4发 0不需要 1需要 x86必须为1
        uint8_t sngl : 1;       // 0级联模式 1单片模式
        uint8_t adi : 1;        // 必须为0
        uint8_t ltim : 1;       // 0边沿触发 1电平触发
        uint8_t ic1_flag : 1;   // 这个必须为1
        uint8_t bits : 3;       // 必须为0
    };
    uint8_t raw;
} ICW1_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union ICW2
{
    struct
    {
        uint8_t bits : 3; // 必须为0
        uint8_t base : 5;
    };
    uint8_t raw;
} ICW2_t;
#pragma pack(pop)

/*
 * 仅当ICW1为级联模式时有效 如果是主片的话 如果1的话 就是中断线级联了其它
 * 如果是从片的话就是低三位为从片接在主片的位置
 *
 */
#pragma pack(push, 1)
typedef union ICW3
{
    struct
    {
        uint8_t irq0 : 1;
        uint8_t irq1 : 1;
        uint8_t irq2 : 1;
        uint8_t irq3 : 1;
        uint8_t irq4 : 1;
        uint8_t irq5 : 1;
        uint8_t irq6 : 1;
        uint8_t irq7 : 1;
    };
    struct
    {
        uint8_t num : 3;
        uint8_t bits : 5;
    };
    uint8_t raw;
} ICW3_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef union ICW4
{
    struct
    {
        uint8_t upm : 1;  // 0为8085模式 1为8086模式
        uint8_t aeoi : 1; // 0 正常EOI模式 1为自动EOI模式
        uint8_t m_s : 1;  // 写0
        uint8_t buf : 1;  // 写0
        uint8_t sfnm : 1; // 0为正常全嵌套 1为特殊全嵌套
        uint8_t bits : 3; // 全为0
    };
    uint8_t raw;
} ICW4_t;
#pragma pack(pop)

void pic_init();
void pic_eoi();

#endif
