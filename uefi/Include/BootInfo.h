#ifndef _BOOTINFO_H_
#define _BOOTINFO_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

// 这个做双重处理（一个是在python脚本中 另一个是是MyBootloader中再对齐）
#define KERNELSIZE 0x6000

typedef struct SystemPageTableInfo
{
    EFI_PHYSICAL_ADDRESS PML4PhysicalBaseAddr;
} SystemPageTableInfoHead;

typedef struct RedirectMapInfo
{
    EFI_PHYSICAL_ADDRESS BaseAddr;
    UINTN MemorySize;
    UINTN DescriptorSize;
} RedirectMapInfoHead;

typedef struct KernelMapInfo
{
    EFI_PHYSICAL_ADDRESS KernelLoadAddr;
    UINTN KernelPages;
} KernelMapInfoHead;

typedef struct VGAInfo
{
    UINTN PixelFormat;
    UINTN HorizontalResolution;
    UINTN VerticalResolution;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    UINTN FrameBufferSize;
    UINTN PixelsPerScanLine;
} VGAInfoHead;

typedef struct BootInfo
{
    // 硬件基础信息
    UINTN CpuMode;
    UINTN AllOfPages;
    SystemPageTableInfoHead SystemPageTable;
    RedirectMapInfoHead MemoryInfo;
    KernelMapInfoHead KernelInfo;
    EFI_PHYSICAL_ADDRESS BootAcpiPhysicalAddress;

    // 显卡基础信息
    VGAInfoHead VGAInfo;

    // 给OS提供的RuntimeServices的入口地址
    EFI_SYSTEM_TABLE *SystemTable;
} BootInfoHead;

#endif
