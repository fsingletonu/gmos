#ifndef _FAT32_H_
#define _FAT32_H_

#include <datatype.h>
#include <drivers/blk_drv/disk.h>
#include <lib/math.h>
#include <lib/string.h>
#include <mm/slab.h>

#define BOOT_SECTOR_LBA 0
#define FSINFO_SECTOR_LBA 1
#define ROOT_DIRECTORY_CLUSTER 2

#define FREE_CLUSTER 0
#define RESERVED_CLUSTER 1
#define BAD_CLUSTER 0x0ffffff7
#define EOF_CLUSTER 0x0fffffff

#define ROOT_DIRECTORY_NAME "FATROOT "
#define FAT32_DIR "DIR"

#define ATTRIBUTE_DIR 0x10
#define ATTRIBUTE_FILE 0x00

#define PAGE_ORDER 12

#define ROOT_CLUSTER 2

#define ROOT_DIR_INIT_CLUSTER 1

#define FILE_STATUS int32_t

#pragma pack(push, 1)
typedef struct fat32_bootsector
{
    uint8_t jump[3];             // 0x00: 跳转指令 (EB 58 90)
    uint8_t oem[8];              // 0x03: OEM 名称 (如 "MSWIN4.1")
    uint16_t bytes_per_sector;   // 0x0B: 每扇区字节数 (通常 512)
    uint8_t sectors_per_cluster; // 0x0D: 每簇扇区数 (1, 2, 4, 8, 16, 32, 64, 128)
    uint16_t reserved_sectors;   // 0x0E: 保留扇区数 (通常 32)
    uint8_t num_fats;            // 0x10: FAT 表数量 (通常 2)
    uint16_t root_entries;       // 0x11: 根目录项数 (FAT32 中通常为 0)
    uint16_t total_sectors_16;   // 0x13: 总扇区数 (如果为 0，则用 total_sectors_32)
    uint8_t media_descriptor;    // 0x15: 介质描述符 (0xF8 表示硬盘)
    uint16_t sectors_per_fat_16; // 0x16: 每个 FAT 表的扇区数 (FAT32 中通常为 0)
    uint16_t sectors_per_track;  // 0x18: 每磁道扇区数
    uint16_t heads;              // 0x1A: 磁头数
    uint32_t hidden_sectors;     // 0x1C: 隐藏扇区数
    uint32_t total_sectors_32;   // 0x20: 总扇区数 (如果 total_sectors_16 == 0)
    uint32_t sectors_per_fat_32; // 0x24: 每个 FAT 表的扇区数 (关键)
    uint16_t extended_flags;     // 0x28: 扩展标志
    uint16_t fs_version;         // 0x2A: 文件系统版本 (0)
    uint32_t root_cluster;       // 0x2C: 根目录的起始簇号 (关键)
    uint16_t fs_info;            // 0x30: FSINFO 扇区号
    uint16_t backup_boot;        // 0x32: 备份引导扇区号
    uint8_t reserved[12];        // 0x34: 保留
    uint8_t drive_number;        // 0x40: 驱动器号
    uint8_t reserved1;           // 0x41: 保留
    uint8_t boot_signature;      // 0x42: 扩展引导签名 (0x29)
    uint32_t volume_id;          // 0x43: 卷序列号
    uint8_t volume_label[11];    // 0x47: 卷标
    uint8_t fs_type[8];          // 0x52: 文件系统类型 ("FAT32   ")
    uint8_t code[420];           // 0x5A: 引导代码
    uint16_t signature;          // 0x1FE: 0xAA55
} fat32_bootsector_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fat32_fsinfo
{
    uint32_t lead_signature;   // 0x41615252 ("RRaA")
    uint8_t reserved1[480];    // 全 0
    uint32_t struct_signature; // 0x61417272 ("rrAa")
    uint32_t free_count;       // 空闲簇数量（-1 = unknown）
    uint32_t next_free;        // 下一个可用簇（hint）
    uint8_t reserved2[12];     // 全 0
    uint32_t trail_signature;  // 0xAA550000（注意低位0）
} fat32_fsinfo_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct fat32_dir_entry
{
    uint8_t name[8];            // 文件名（不足补空格，8 字节）
    uint8_t ext[3];             // 扩展名（不足补空格，3 字节）
    uint8_t attributes;         // 文件属性（见下）
    uint8_t reserved;           // 保留 (NT 用)
    uint8_t create_time_tenths; // 创建时间（厘秒）
    uint16_t create_time;       // 创建时间
    uint16_t create_date;       // 创建日期
    uint16_t access_date;       // 最后访问日期
    uint16_t cluster_high;      // 起始簇号高 16 位
    uint16_t modify_time;       // 修改时间
    uint16_t modify_date;       // 修改日期
    uint16_t cluster_low;       // 起始簇号低 16 位
    uint32_t file_size;         // 文件大小（字节）
} fat32_dir_entry_t;
#pragma pack(pop)

typedef struct fat32_global_fat
{
    uint32_t flags;
    uint32_t fat_entry_total_count;
    phys_addr global_fat_addr;
} fat32_global_fat_t;

typedef struct fat32_entry_link
{
    uint32_t cluster_num;
    uint64_t offset_in_cluster;
    struct fat32_entry_link *prev;
    struct fat32_entry_link *next;
} fat32_entry_link_t;

extern fat32_global_fat_t global_fat;

void fat32_fill_bootsectors(mmzone_t *zone);
void fat32_fill_fat(mmzone_t *zone);
void fat32_mount(mmzone_t *zone);
FILE_STATUS get_fat32_status(mmzone_t *zone);

void fat_global_init();
void fat32_global_fat_init(mmzone_t *zone);
uint32_t get_free_cluster(fat32_global_fat_t fat);

phys_addr fat32_open_root_dir();
void fat32_close_root_dir();

#endif
