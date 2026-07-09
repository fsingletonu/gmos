/*
 * ide.h
 * (C) Guo Fu
 * 这个部分就按保护模式下的写了 如果之后要兼容长模式 那就重新新的源文件
 * 每个扇区都需要要等待 且规定count为 0 则是一次读取256个扇区
 */
#ifndef _IDE_H_
#define _IDE_H_

#include <datatype.h>
#include <chr_drv/io.h>
#include <init/i386.h>
#include <init/pci.h>

#define DATA_MASTER_PORT 0x1f0
#define DATA_SLAVE_PORT 0x170
#define ERROR_FUNCTION_PORT 0x1f1
#define COUNT_PORT 0x1f2
#define LBA_LOW_PORT 0x1f3
#define LBA_MID_PORT 0x1f4
#define LBA_HIGH_PORT 0x1f5
#define DEVICE_MASTER_PORT 0x1f6
#define DEVICE_SLAVE_PORT 0x176
#define STATUS_COMMAND_MASTER_PORT 0x1f7
#define STATUS_COMMAND_SLAVE_PORT 0x177

#define CONTROL_PORT 0x3f6
#define ALTERNATE_STATUS 0x3f7

/*
IDE 控制器硬件使能中断
IDE 有一个 控制寄存器（0x3F6 或 0x376，Device Control Register），位如下：
位	名称	功能
7	nIEN	0 = 允许中断，1 = 禁止中断
6	SRST	设备复位
5	–	保留
4	–	保留
3	–	保留
2	–	保留
1	–	保留
0	–	保留
*/

/*
状态寄存器（读 0x1F7）
状态寄存器的每一位都有固定意义：
位	名称	含义
7	BSY	Busy，1 = 硬盘正在执行命令
6	DRDY	Drive Ready，1 = 硬盘就绪
5	DF	Device Fault，1 = 硬盘故障
4	DSC	Drive Seek Complete，1 = 硬盘寻道完成
3	DRQ	Data Request，1 = 数据可读/可写
2	CORR	Corrected Data，1 = ECC 纠正成功
1	IDX	Index，一般无用，磁盘旋转索引
0	ERR	Error，1 = 最近命令发生错误
*/

enum device_ide_type
{
    IDE_TYPE_MASTER = 0x00,
    IDE_TYPE_SLAVE = 0x10
};

enum lba_mode
{
    NO_LBA = 0,
    LBA_28,
    LBA_48
};

enum work_mode
{
    WORK_READ_SECTIONS_28_POLLING = 0x20,
    WORK_WRITE_SECTIONS_28_POLLING = 0x30,
    WORK_READ_SECTIONS_28 = 0x20,
    WORK_READ_SECTIONS_48 = 0x21,
    WORK_WRITE_SECTIONS_28 = 0x30,
    WORK_WRITE_SECTIONS_48 = 0x31,
    WORK_READ_IDE_DMA = 0xc8,
    WORK_WRITE_IDE_DMA = 0xca
};

#define IDE_MODE_CHS 0x00
#define IDE_MODE_LBA 0x10

#define READ_SECTIONS_28 0x20
#define READ_SECTIONS_48 0x21
#define WRITE_SECTIONS_28 0x30
#define WRITE_SECTIONS_48 0x31
#define READ_DMA 0xc8
#define WRITE_DMA 0xca
#define IDE_IDENTIFY 0xec
#define IDE_SEEK 0xf7
#define IDE_DIAGNOSTIC 0x90
#define SET_FEATURES 0xef // 功能号写在0x1f0中
#define SET_FEATURES_FUCTIONS_CACHE 0x01
#define SET_FEATURES_FUCTIONS_OVERLAP 0x02
#define SET_FEATURES_FUCTIONS_SMART 0x03
#define SET_FEATURES_FUCTIONS_POWER_MANAGEMENT 0x05
#define SET_FEATURES_FUCTIONS_PIO_DMA 0x08
#define SET_FEATURES_FUCTIONS_SECURITY_UNLOCK 0xaa
#define SET_FEATURES_FUCTIONS_SECURITY_LOCK 0x55
#define FLUSH_CAHCE 0xe7

// 仅参考用
typedef struct storage_mess_struct
{
    uint16_t general_config; // word 0

    uint16_t obsolete1[9]; // word 1–9

    char serial_number[20]; // word 10–19

    uint16_t obsolete2[3]; // word 20–22

    char firmware_revision[8]; // word 23–26
    char model_number[40];     // word 27–46

    uint16_t max_sectors_per_drq; // word 47
    uint16_t trusted_computing;   // word 48

    uint16_t capabilities[2]; // word 49–50
    uint16_t obsolete3[2];    // word 51–52
    uint16_t field_validity;  // word 53

    uint16_t current_cylinders;         // word 54
    uint16_t current_heads;             // word 55
    uint16_t current_sectors_per_track; // word 56
    uint32_t current_capacity;          // word 57–58

    uint16_t multi_sector_setting; // word 59

    uint32_t lba28_total_sectors; // word 60–61

    uint16_t obsolete4; // word 62

    uint16_t dma_modes; // word 63
    uint16_t pio_modes; // word 64

    uint16_t min_dma_cycle;         // word 65
    uint16_t rec_dma_cycle;         // word 66
    uint16_t min_pio_cycle_no_flow; // word 67
    uint16_t min_pio_cycle_iordy;   // word 68

    uint16_t reserved1[6]; // word 69–74

    uint16_t queue_depth; // word 75

    uint16_t sata_capabilities;       // word 76
    uint16_t sata_reserved;           // word 77
    uint16_t sata_features_supported; // word 78
    uint16_t sata_features_enabled;   // word 79

    uint16_t major_version; // word 80
    uint16_t minor_version; // word 81

    uint16_t command_sets_supported[3]; // word 82–84
    uint16_t command_sets_enabled[3];   // word 85–87

    uint16_t ultra_dma_modes; // word 88

    uint16_t security_erase_time; // word 89
    uint16_t enhanced_erase_time; // word 90

    uint16_t current_apm;        // word 91
    uint16_t master_password_id; // word 92

    uint16_t hw_reset_result;     // word 93
    uint16_t acoustic_management; // word 94

    uint16_t stream_min_req_size;     // word 95
    uint16_t stream_transfer_time;    // word 96
    uint16_t stream_access_latency;   // word 97
    uint32_t stream_perf_granularity; // word 98–99

    uint64_t lba48_total_sectors; // word 100–103

    uint16_t stream_transfer_time_dma; // word 104
    uint16_t phys_log_sector_size;     // word 106

    uint16_t interseek_delay; // word 107

    uint16_t world_wide_name[4]; // word 108–111

    uint16_t reserved2[10]; // word 112–121

    uint16_t removable_status; // word 122
    uint16_t security_status;  // word 128

    uint16_t reserved3[127]; // word 129–255

} storage_info_struct_t;

typedef struct storage_mode_size_struct
{
    enum lba_mode lba_mode;
    uint32_t sector_size;
    uint64_t total_sectors;
} storage_mode_size_struct_t;
storage_mode_size_struct_t storage_base_mess;

// 块起始位置必须是物理地址
typedef struct prdt
{
    phys_addr_t data_addr;
    uint16_t byte_count;
    uint16_t eot_flags;
} _packed prdt_t;
prdt_t prdt;

/*
 * 1、command寄存器：
 * 第 0 位：1 为开始DMA 0 为停止DMA；第 3 位：1 为读 0 为写 剩下全是零。
 *
 * 2、status寄存器：
 * 第 0 位（active）：1 为DMA正在进行；第 1 位（error）：1 为发生错误；
 * 第 2 位（interrupt）：1 为DMA正确产生中断；第 5 位：从设备支持dma；
 * 第 6 位：主设备支持dma。
 *
 * 3、prdt address寄存器：
 * 给结构体的真实物理地址。
 */

#define IDE_DMA_READ 0x08
#define IDE_DMA_WRITE 0x00

typedef struct bus_master_ide_dma_r
{
    uint16_t master_command_r;
    uint16_t master_status_r;
    uint16_t master_prdt_addr_r;
    uint16_t slave_command_r;
    uint16_t slave_status_r;
    uint16_t slave_prdt_addr_r;
} bus_master_ide_dma_r_t;
bus_master_ide_dma_r_t ide_dma_r;

void enable_ide_int();
void disable_ide_init();
void get_base_storage_mess();
void ide_init();
void send_ide_request(uint32_t lba_addr, uint8_t count, enum device_ide_type type, enum work_mode work, phys_addr_t data_area);
void cpu_read_write_storage();
void set_ide_dma_r(uint8_t command, phys_addr_t prdt_addr);

#endif

/*
 * 大块数据拆分成相同的块进行传输 都用DMA方式
 * 每个块最多不超过64k
 */

/*
 * 读取磁盘错误处理：
 * 1、重试几次
 * 2、返回错误码
 * 3、错误回调任务队列（想去实现）
 */