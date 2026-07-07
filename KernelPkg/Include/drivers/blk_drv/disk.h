#ifndef _DISK_H_
#define _DISK_H_

/*
 * 目前的磁盘驱动也是没辙了，PCIe实在整不出来，作者自己也编译固件了，也直接用了Ubuntu自带的OVMF.fd
 * 还有QEMU中也info qtree 确定就是有pcie.0 但就是acpi找不到MCFG
 * 作者不想固定ECAM一定为0xe00000 作者只想运行时确定ECAM的基地址 所以暂时就不适配PCIe了
 * 主要作者不想弄坏工控机 所以 虚拟机整不出来 实体机也就不开发和测试了
 */
#include <datatype.h>
#include <drivers/chr_drv/iox64.h>
#include <drivers/chr_drv/iox86.h>
#include <mm/zone.h>
#include <lib/string.h>

// 这里只有DATA_PORT为16位端口 PCIe的模拟出问题了
// 0x1f2写0代表R/W 256扇区
#define LBA_DATA_PORT 0x1f0
#define LBA_STATUS_PORT 0x1f1
#define LBA_SECTOR_COUNT_PORT 0x1f2
#define LBA_BLOCK_LOW_PORT 0x1f3
#define LBA_BLOCK_MID_PORT 0x1f4
#define LBA_BLOCK_HIGH_PORT 0x1f5
#define LBA_MORE_DISK_PORT 0x1f6
#define LBA_COMMAND_PORT 0x1f7

#define LBA_IDENTIFY_DEVICE_COMMAND 0xec
#define LBA28_READ_COMMAND 0x20
#define LBA48_READ_COMMAND 0x24
#define LBA28_WRITE_COMMAND 0x30
#define LBA48_WRITE_COMMAND 0x34
#define FLUSH_CACHE_COMMAND 0xe7

#define LBA28_MASTER_DISK 0xe0
#define LBA28_SLAVE_DISK 0xf0

#define DISK_STATUS int32_t

#pragma pack(push, 1)
typedef struct ata_identify_device
{
    uint16_t word0;
    uint16_t reserved1[9];    // 1-9
    uint16_t serial[10];      // 10-19
    uint16_t reserved2[3];    // 20-22
    uint16_t firmware[4];     // 23-26
    uint16_t model[20];       // 27-46
    uint16_t max_rw_sectors;  // 47
    uint16_t reserved48;      // 48
    uint16_t capabilities[2]; // 49-50
    uint16_t reserved3[9];    // 51-59
    uint32_t lba28_sectors;   // 60-61
    uint16_t reserved4[38];   // 62-99
    uint64_t lba48_sectors;   // 100-103
    uint16_t reserved5[152];  // 104-255
} ata_identify_device_t;
#pragma pack(pop)

void delay_400ns();
uint8_t read_status();
uint16_t read_data();
void write_data(uint16_t data);
void wait_not_busy();
void wait_disk_ready();
void wait_data_ready_request();
void pre_send_command();
void after_send_command();
void send_identify_device_command();
phys_addr read_identify_device(mmzone_t *zone);
uint32_t get_sector_bytes(phys_addr addr);
void lba28_select(uint64_t lba_num);

// 读磁盘函数只负责写入内存，申请内存则交给调用者56
DISK_STATUS read_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device);
DISK_STATUS write_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device);

DISK_STATUS read_more_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device);
DISK_STATUS write_more_lba28_sectors(void *buffer, uint64_t lba_num, uint64_t count, uint8_t m_s_device);

#endif
