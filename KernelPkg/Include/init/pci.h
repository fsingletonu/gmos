#ifndef _PCI_H_
#define _PCI_H_

#include <datatype.h>

#define PCI_CONF_ADDR 0xcf8
#define PCI_CONF_DATA 0xcfc

#pragma pack(push, 1)
typedef struct pci_configSpace
{
    // 0x00 ~ 0x0F 标准头部
    uint16_t vendor_id;      // 0x00
    uint16_t device_id;      // 0x02
    uint16_t command;        // 0x04
    uint16_t status;         // 0x06
    uint8_t revision_id;     // 0x08
    uint8_t prog_if;         // 0x09
    uint8_t subclass;        // 0x0A
    uint8_t class;           // 0x0B
    uint8_t cache_line_size; // 0x0C
    uint8_t latency_timer;   // 0x0D
    uint8_t header_type;     // 0x0E
    uint8_t bist;            // 0x0F

    // 0x10 ~ 0x3F 标准头扩展
    uint32_t bar[6];                // 0x10 ~ 0x27 (BAR0~BAR5)
    uint32_t cardbus_cis_ptr;       // 0x28
    uint16_t subsystem_vendor_id;   // 0x2C
    uint16_t subsystem_id;          // 0x2E
    uint32_t expansion_rom_base;    // 0x30
    uint8_t capabilities_ptr;       // 0x34
    uint8_t reserved1[3];           // 0x35 ~ 0x37
    uint32_t reserved2;             // 0x38 ~ 0x3B
    uint8_t interrupt_line;         // 0x3C
    uint8_t interrupt_pin;          // 0x3D
    uint16_t min_grant_max_latency; // 0x3E ~ 0x3F

    // 0x40 ~ 0xFF 设备扩展区（设备自定义 / 保留）
    uint8_t device_specific[192]; // 0x40 ~ 0xFF
} pci_configSpace_t;
#pragma pack(pop)

#endif
