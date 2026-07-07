#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Guid/Acpi.h>
#include <Protocol/GraphicsOutput.h>
#include "Include/CpuMode.h"
#include "Include/BootInfo.h"
#include "Include/MemoryMap.h"
#include "Include/VGAInfo.h"

EFI_STATUS
EFIAPI
MyBootUefiMain(IN EFI_HANDLE ImageHandle,
               IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    BootInfoHead BootInfo;

    SystemTable->BootServices->SetMem(&BootInfo, sizeof(BootInfo), 0);

    BootInfo.CpuMode = GetCpuMode();

    Print(L"GMOS BootLoader...\r\n");

    UINTN MemorySize = 0;
    Status = GetSMBIOSMemorySize(SystemTable,
                                 &MemorySize);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Print(L"MemorySize: 0x%x KB\r\n", MemorySize);

    BootInfo.AllOfPages = MemorySize / 4;

    Status = GetPML4Table(SystemTable,
                          &BootInfo);

    Print(L"PML4PhysicalBaseAddr: %lx\r\n", BootInfo.SystemPageTable.PML4PhysicalBaseAddr);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    // 恒等映射物理内存
    Status = MapMemoryRegion(SystemTable,
                             &BootInfo,
                             (EFI_PHYSICAL_ADDRESS)0,
                             (EFI_PHYSICAL_ADDRESS)0,
                             MemorySize * 1024,
                             MemoryType_IdentityMemory);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Status = GetVGAInfo(SystemTable,
                        &BootInfo);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    // 直接映射显存
    Status = MapMemoryRegion(SystemTable,
                             &BootInfo,
                             (EFI_PHYSICAL_ADDRESS)BootInfo.VGAInfo.FrameBufferBase,
                             (EFI_PHYSICAL_ADDRESS)(BootInfo.VGAInfo.FrameBufferBase + SystemPageTableDirectMapping),
                             BootInfo.VGAInfo.FrameBufferSize,
                             MemoryType_DirectVGAMemory);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Status = KernelLoading(SystemTable,
                           &BootInfo);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Print(L"KernelLoadAddr: %x\r\n", BootInfo.KernelInfo.KernelLoadAddr);

    // 高地址映射内核
    Status = MapMemoryRegion(SystemTable,
                             &BootInfo,
                             (EFI_PHYSICAL_ADDRESS)BootInfo.KernelInfo.KernelLoadAddr,
                             (EFI_PHYSICAL_ADDRESS)KernelVirtualBaseAddressX64,
                             BootInfo.KernelInfo.KernelPages * 4096,
                             MemoryType_KernelMemory);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Status = GetRSDPTR(SystemTable,
                       &BootInfo);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    EFI_PHYSICAL_ADDRESS KeepPointerBootInfo;
    Status = SystemTable->BootServices->AllocatePool(EfiRuntimeServicesData,
                                                     sizeof(BootInfoHead),
                                                     (VOID **)&KeepPointerBootInfo);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    SystemTable->BootServices->CopyMem((VOID *)KeepPointerBootInfo, &BootInfo, sizeof(BootInfoHead));

    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MemoryMapSize = 0;
    UINTN MapKey = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;

    Status = SystemTable->BootServices->GetMemoryMap(&MemoryMapSize,
                                                     MemoryMap,
                                                     &MapKey,
                                                     &DescriptorSize,
                                                     &DescriptorVersion);

    if (Status != EFI_BUFFER_TOO_SMALL)
    {
        SystemTable->BootServices->Stall(3000000);
        SystemTable->RuntimeServices->ResetSystem(
            EfiResetCold,
            EFI_SUCCESS,
            0,
            NULL);
        return EFI_SUCCESS;
    }

    MemoryMapSize += 2 * DescriptorSize;

    Status = SystemTable->BootServices->AllocatePool(EfiRuntimeServicesData,
                                                     MemoryMapSize,
                                                     (VOID **)&MemoryMap);

    if (EFI_ERROR(Status))
    {
        SystemTable->BootServices->Stall(3000000);
        SystemTable->RuntimeServices->ResetSystem(
            EfiResetCold,
            EFI_SUCCESS,
            0,
            NULL);
        return EFI_SUCCESS;
    }

    Status = SystemTable->BootServices->GetMemoryMap(&MemoryMapSize,
                                                     MemoryMap,
                                                     &MapKey,
                                                     &DescriptorSize,
                                                     &DescriptorVersion);

    if (EFI_ERROR(Status))
    {
        SystemTable->BootServices->Stall(3000000);
        SystemTable->RuntimeServices->ResetSystem(
            EfiResetCold,
            EFI_SUCCESS,
            0,
            NULL);
        return EFI_SUCCESS;
    }

    BootInfoHead *ptr = (BootInfoHead *)KeepPointerBootInfo;

    ptr->MemoryInfo.BaseAddr = (EFI_PHYSICAL_ADDRESS)MemoryMap;
    ptr->MemoryInfo.MemorySize = MemoryMapSize;
    ptr->MemoryInfo.DescriptorSize = DescriptorSize;

    Status = SystemTable->BootServices->ExitBootServices(ImageHandle,
                                                         MapKey);

    if (EFI_ERROR(Status))
    {
        Print(L"failed\r\n");
        SystemTable->BootServices->Stall(3000000);
        SystemTable->RuntimeServices->ResetSystem(
            EfiResetCold,
            EFI_SUCCESS,
            0,
            NULL);
        return EFI_SUCCESS;
    }

    AsmWriteCr3(BootInfo.SystemPageTable.PML4PhysicalBaseAddr);

    asm volatile("mov %0, %%rbx\n\t"
                 :
                 : "r"(KeepPointerBootInfo)
                 : "rbx", "memory");

    typedef void (*KernelEntry)();
    KernelEntry entry = (KernelEntry)KernelVirtualBaseAddressX64;
    entry();

    while (1)
        ;

    return EFI_SUCCESS;
}