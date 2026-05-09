#ifndef _MEMORYMAPDESC_H_
#define _MEMORYMAPDESC_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include "Include/CpuMode.h"
#include "Include/BootInfo.h"
#include <Library/BaseMemoryLib.h>

// 这里妥协使用硬编码去确定物理内存实际大小
#define PhysicalMemoryRealSize 0x10000000
// #define PhysicalMemoryRealSize 0x100000000

// 虚拟地址有效位为48位 物理地址有效位为52位
#define KernelVirtualBaseAddressX86 0xfff00000
#define KernelVirtualBaseAddressX64 0xffffffff80000000

#define SystemPageTableDirectMapping 0xffff800000000000

typedef enum
{
    MemoryMap_PDEEntry,
    MemoryMap_PTEEntry,
    MemoryMap_PML4Entry,
    MemoryMap_PDPTEntry,
    MemoryMap_PDEntry,
    MemoryMap_PTEntry
} EntryType;

typedef enum
{
    MemoryType_IdentityMemory,
    MemoryType_DirectMemory,
    MemoryType_DirectVGAMemory,
    MemoryType_KernelMemory
} MemoryType;

#pragma pack(1)
typedef struct PDE
{
    UINT8 P : 1;                 // 存在位 (Present)
    UINT8 R_W : 1;               // 读写权限 (Read/Write)
    UINT8 U_S : 1;               // 用户/超级用户权限 (User/Supervisor)
    UINT8 PWT : 1;               // 写透 (Write-Through)
    UINT8 PCD : 1;               // 缓存禁用 (Cache Disable)
    UINT8 A : 1;                 // 访问位 (Accessed)
    UINT8 D : 1;                 // 赃位（仅对大页PDE有效）
    UINT8 PS : 1;                // 页大小 (Page Size)
    UINT8 G : 1;                 // 全局位（Global，仅对大页PDE有效）
    UINT8 AVL : 3;               // 可供操作系统使用
    UINT32 page_table_base : 20; // 页表基地址 (Page Table Base Address)
} PDE_ENTRY;                     // 页目录项

#pragma pack(1)
typedef struct
{
    UINT8 P : 1;                 // 存在位
    UINT8 R_W : 1;               // 读写权限
    UINT8 U_S : 1;               // 用户/超级用户
    UINT8 PWT : 1;               // 写透
    UINT8 PCD : 1;               // 缓存禁用
    UINT8 A : 1;                 // 访问位
    UINT8 D : 1;                 // 脏位
    UINT8 PS : 1;                // 页大小
    UINT8 G : 1;                 // 全局页
    UINT8 reserve : 3;           // 保留位
    UINT32 page_frame_base : 20; // 物理页框基地址
} PTE_ENTRY;                     // 这个数据结构为四个字节，页表项

#pragma pack(1)
typedef union
{
    struct
    {
        UINT64 Present : 1;        // bit 0
        UINT64 ReadWrite : 1;      // bit 1
        UINT64 UserSuper : 1;      // bit 2
        UINT64 WriteThrough : 1;   // bit 3
        UINT64 CacheDisabled : 1;  // bit 4
        UINT64 Accessed : 1;       // bit 5
        UINT64 Ignored1 : 1;       // bit 6
        UINT64 MustBeZero : 1;     // bit 7，必须为 0
        UINT64 Ignored2 : 4;       // bits 8-11
        UINT64 PhysicalAddr : 40;  // bits 12-51，物理地址（右移 12 位）
        UINT64 Ignored3 : 11;      // bits 52-62
        UINT64 ExecuteDisable : 1; // bit 63，NX
    } bits;
    UINT64 raw;
} PML4_ENTRY;

#pragma pack(1)
typedef union
{
    struct
    {
        UINT64 Present : 1;
        UINT64 ReadWrite : 1;
        UINT64 UserSuper : 1;
        UINT64 WriteThrough : 1;
        UINT64 CacheDisabled : 1;
        UINT64 Accessed : 1;
        UINT64 Ignored1 : 1;
        UINT64 MustBeZero : 1;    // bit 7，必须为 0
        UINT64 Ignored2 : 4;      // bits 8-11
        UINT64 PhysicalAddr : 40; // bits 12-51
        UINT64 Ignored3 : 11;     // bits 52-62
        UINT64 ExecuteDisable : 1;
    } bits;
    UINT64 raw;
} PDPT_ENTRY;

#pragma pack(1)
typedef union
{
    struct
    {
        UINT64 Present : 1;
        UINT64 ReadWrite : 1;
        UINT64 UserSuper : 1;
        UINT64 WriteThrough : 1;
        UINT64 CacheDisabled : 1;
        UINT64 Accessed : 1;
        UINT64 Ignored1 : 1;
        UINT64 PageSize : 1;      // bit 7，大页标志（2MB）
        UINT64 Ignored2 : 4;      // bits 8-11
        UINT64 PhysicalAddr : 40; // bits 12-51
        UINT64 Ignored3 : 11;     // bits 52-62
        UINT64 ExecuteDisable : 1;
    } bits;
    UINT64 raw;
} PD_ENTRY;

#pragma pack(1)
typedef union
{
    struct
    {
        UINT64 Present : 1;
        UINT64 ReadWrite : 1;
        UINT64 UserSuper : 1;
        UINT64 WriteThrough : 1;
        UINT64 CacheDisabled : 1;
        UINT64 Accessed : 1;
        UINT64 Dirty : 1;         // bit 6，已写入
        UINT64 MustBeZero : 1;    // bit 7，必须为 0
        UINT64 Global : 1;        // bit 8，全局页
        UINT64 Ignored : 3;       // bits 9-11
        UINT64 PhysicalAddr : 40; // bits 12-51，指向 4KB 物理页
        UINT64 Ignored2 : 11;     // bits 52-62
        UINT64 ExecuteDisable : 1;
    } bits;
    UINT64 raw;
} PT_ENTRY;

// 在工控机上先弃用 工控机固件太老了 AMI的古早固件根本不支持五参数的 tianocore的支持 单位：KB
EFI_STATUS GetSMBIOSMemorySize(IN EFI_SYSTEM_TABLE *SystemTable,
                               OUT UINTN *MemorySize);

EFI_STATUS GetPML4Table(IN EFI_SYSTEM_TABLE *SystemTable,
                        IN OUT BootInfoHead *Info);

EFI_STATUS KernelLoading(IN EFI_SYSTEM_TABLE *SystemTable,
                         OUT BootInfoHead *Info);

// 以4k为一页
EFI_STATUS MapMemoryRegion(IN EFI_SYSTEM_TABLE *SystemTable,
                           IN OUT BootInfoHead *Info,
                           IN EFI_PHYSICAL_ADDRESS PhysicalStart,
                           IN EFI_PHYSICAL_ADDRESS VirtualStart,
                           IN UINTN Size,
                           IN MemoryType Type);

EFI_STATUS GetRSDPTR(IN EFI_SYSTEM_TABLE *SystemTable,
                     OUT BootInfoHead *Info);

#endif
