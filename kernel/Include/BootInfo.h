// 一定要保证此处的结构同BIOS中的和内核中的结构一致 要不然 测试不了（图形化GDB有点傻

#ifndef _BOOTINFO_H_
#define _BOOTINFO_H_

#include <datatype.h>

typedef enum
{
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory
} EfiMemoryDescriptorType;

typedef struct SystemPageTableInfo
{
    uint64_t PML4PhysicalBaseAddr;
} SystemPageTableInfoHead;

typedef struct RedirectMapInfo
{
    uint64_t BaseAddr;
    uint64_t MemorySize;
    uint64_t DescriptorSize;
} RedirectMapInfoHead;

typedef struct KernelMapInfo
{
    uint64_t KernelLoadAddr;
    uint64_t KernelPages;
} KernelMapInfoHead;

typedef struct VGAInfo
{
    uint64_t PixelFormat;
    uint64_t HorizontalResolution;
    uint64_t VerticalResolution;
    uint64_t FrameBufferBase;
    uint64_t FrameBufferSize;
    uint64_t PixelsPerScanLine;
} VGAInfoHead;

typedef struct BootInfo
{
    // 硬件基础信息
    uint64_t CpuMode;
    uint64_t AllOfPages;
    SystemPageTableInfoHead SystemPageTable;
    RedirectMapInfoHead MemoryInfo;
    KernelMapInfoHead KernelInfo;
    uint64_t BootAcpiPhysicalAddress;

    // 显卡基础信息
    VGAInfoHead VGAInfo;

    // 给OS提供的RuntimeServices的入口地址
    uint64_t *SystemTable;
} BootInfoHead;

// BIOS的内存描述符
typedef struct ards
{
    uint64_t base;
    uint64_t limit;
    uint32_t type;
} ards_t;

// UEFI的内存描述符
typedef struct
{
    uint32_t Type;
    uint32_t Pad;
    uint64_t PhysicalStart;
    uint64_t VirtualStart;
    uint64_t NumberOfPages;
    uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

#endif
