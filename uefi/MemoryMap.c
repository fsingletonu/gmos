#include "Include/MemoryMap.h"

EFI_STATUS GetSMBIOSMemorySize(IN EFI_SYSTEM_TABLE *SystemTable,
                               OUT UINTN *MemorySize)
{
#if USE_SMBIOS_MEMORY_DETECTION
    EFI_STATUS Status;
    EFI_SMBIOS_PROTOCOL *Smbios;
    EFI_SMBIOS_HANDLE SmbiosHandle;
    EFI_SMBIOS_TYPE Type;
    EFI_SMBIOS_TABLE_HEADER *Record;
    EFI_HANDLE ProducerHandle;

    *MemorySize = 0;

    Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid,
                                 NULL,
                                 (VOID **)&Smbios);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_MEMORY_DEVICE;

    for (Status = Smbios->GetNext(Smbios, &SmbiosHandle, &Type, &Record, &ProducerHandle);
         !EFI_ERROR(Status);
         Status = Smbios->GetNext(Smbios, &SmbiosHandle, &Type, &Record, &ProducerHandle))
    {
        SMBIOS_TABLE_TYPE17 *Type17 = (SMBIOS_TABLE_TYPE17 *)Record;
        if (Type17->Size == 0 || Type17->Size == 0xffff)
            continue;
        if (Type17->Size == 0x7fff)
            *MemorySize += Type17->ExtendedSize * 1024;
        else if (Type17->Size & 0x8000)
            *MemorySize += (Type17->Size & 0x7FFF);
        else
            *MemorySize += Type17->Size * 1024;
    }

    if (Status == EFI_NOT_FOUND)
    {
        Status = EFI_SUCCESS;
    }
#else
    *MemorySize = PhysicalMemoryRealSize / 1024;
#endif
    return EFI_SUCCESS;
}

EFI_STATUS GetPML4Table(IN EFI_SYSTEM_TABLE *SystemTable,
                        IN OUT BootInfoHead *Info)
{
    EFI_STATUS Status;
    Status = SystemTable->BootServices->AllocatePages(AllocateAnyPages,
                                                      EfiRuntimeServicesData,
                                                      1,
                                                      &(Info->SystemPageTable.PML4PhysicalBaseAddr));

    if (EFI_ERROR(Status))
    {
        Print(L"Allocate PML4 Failed...\r\n");
        return Status;
    }

    SystemTable->BootServices->SetMem((VOID *)Info->SystemPageTable.PML4PhysicalBaseAddr, 4096, 0);

    return Status;
}

EFI_STATUS KernelLoading(IN EFI_SYSTEM_TABLE *SystemTable,
                         OUT BootInfoHead *Info)
{
    EFI_STATUS Status;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
    EFI_FILE_PROTOCOL *Root;
    EFI_FILE_PROTOCOL *KernelFile;

    UINTN KernelPages = KERNELSIZE >> 12;
    EFI_PHYSICAL_ADDRESS KernelLoadAddr = 0;

    Status = SystemTable->BootServices->AllocatePages(AllocateAnyPages,
                                                      EfiRuntimeServicesData,
                                                      KernelPages,
                                                      &KernelLoadAddr);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    Info->KernelInfo.KernelLoadAddr = KernelLoadAddr;
    Info->KernelInfo.KernelPages = KernelPages;

    Status = SystemTable->BootServices->LocateProtocol(
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        (VOID **)&Fs);

    if (EFI_ERROR(Status))
    {
        Print(L"Get File System Failed\r\n");
        return Status;
    }

    Status = Fs->OpenVolume(Fs, &Root);

    if (EFI_ERROR(Status))
    {
        Print(L"Get File System Root Failed\r\n");
        return Status;
    }

    Status = Root->Open(
        Root,
        &KernelFile,
        L"\\EFI\\KERNEL\\system.bin",
        EFI_FILE_MODE_READ,
        0);

    if (EFI_ERROR(Status))
    {
        Print(L"Open system.bin Failed\r\n");
        return Status;
    }

    Print(L"Open system.bin SUCCESS\r\n");

    UINTN Size = KERNELSIZE;
    Status = KernelFile->Read(
        KernelFile,
        &Size,
        (VOID *)KernelLoadAddr);

    if (EFI_ERROR(Status))
    {
        Print(L"Read system.bin Failed\r\n");
        return Status;
    }

    Print(L"Read system.bin SUCCESS\r\n");

    return EFI_SUCCESS;
}

EFI_STATUS MapMemoryRegion(IN EFI_SYSTEM_TABLE *SystemTable,
                           IN OUT BootInfoHead *Info,
                           IN EFI_PHYSICAL_ADDRESS PhysicalStart,
                           IN EFI_PHYSICAL_ADDRESS VirtualStart,
                           IN UINTN Size,
                           IN MemoryType Type)
{
    EFI_STATUS Status;
    UINTN Pages = (Size + 0xfff) >> 12;
    UINTN PTNumber = (Pages + 511) / 512;
    UINTN PDNumber = (PTNumber + 511) / 512;
    UINTN PDPTNumber = (PDNumber + 511) / 512;
    UINTN AllTablePages = PDPTNumber + PDNumber + PTNumber;
    EFI_PHYSICAL_ADDRESS TableBaseAddr = 0;

    Status = SystemTable->BootServices->AllocatePages(AllocateAnyPages,
                                                      EfiRuntimeServicesData,
                                                      AllTablePages,
                                                      &TableBaseAddr);

    if (EFI_ERROR(Status))
    {
        Print(L"Allocate Pages Failed...\r\n");
        return Status;
    }

    SystemTable->BootServices->SetMem((VOID *)TableBaseAddr, AllTablePages * 4096, 0);

    EFI_PHYSICAL_ADDRESS PDPTBaseAddr = TableBaseAddr;
    EFI_PHYSICAL_ADDRESS PDBaseAddr = TableBaseAddr + PDPTNumber * 4096;
    EFI_PHYSICAL_ADDRESS PTBaseAddr = TableBaseAddr + (PDPTNumber + PDNumber) * 4096;

    UINTN PML4Idx = (VirtualStart >> (12 + 9 + 9 + 9)) & 0x1FF;
    UINTN PDPTIdx = (VirtualStart >> (12 + 9 + 9)) & 0x1FF;
    UINTN PDIdx = (VirtualStart >> (12 + 9)) & 0x1FF;
    UINTN PTIdx = (VirtualStart >> 12) & 0x1FF;

    UINT64 Pat = 0;
    if (Type == MemoryType_DirectVGAMemory)
    {
        Pat = 1;
    }

    // 1、先填充PML4
    PML4_ENTRY *PML4BaseEntry = (PML4_ENTRY *)Info->SystemPageTable.PML4PhysicalBaseAddr;
    for (UINTN i = 0; i < PDPTNumber; i++)
    {
        PML4BaseEntry[PML4Idx + i].bits.Present = 1;
        PML4BaseEntry[PML4Idx + i].bits.ReadWrite = 1;
        PML4BaseEntry[PML4Idx + i].bits.PhysicalAddr = (PDPTBaseAddr >> 12) + i;
    }

    // 2、再填充PDPT
    PDPT_ENTRY *PDPTBaseEntry = (PDPT_ENTRY *)PDPTBaseAddr;
    for (UINTN i = 0; i < PDNumber; i++)
    {
        PDPTBaseEntry[PDPTIdx + i].bits.Present = 1;
        PDPTBaseEntry[PDPTIdx + i].bits.ReadWrite = 1;
        PDPTBaseEntry[PDPTIdx + i].bits.PhysicalAddr = (PDBaseAddr >> 12) + i;
    }

    // 3、再填充PD
    PD_ENTRY *PDBaseEntry = (PD_ENTRY *)PDBaseAddr;
    for (UINTN i = 0; i < PTNumber; i++)
    {
        PDBaseEntry[PDIdx + i].bits.Present = 1;
        PDBaseEntry[PDIdx + i].bits.ReadWrite = 1;
        PDBaseEntry[PDIdx + i].bits.PhysicalAddr = (PTBaseAddr >> 12) + i;
    }

    // 4、最后填充PT
    PT_ENTRY *PTBaseEntry = (PT_ENTRY *)PTBaseAddr;
    for (UINTN i = 0; i < Pages; i++)
    {
        PTBaseEntry[PTIdx + i].bits.Present = 1;
        PTBaseEntry[PTIdx + i].bits.ReadWrite = 1;
        PTBaseEntry[PTIdx + i].bits.MustBeZero = Pat;
        PTBaseEntry[PTIdx + i].bits.PhysicalAddr = (PhysicalStart >> 12) + i;
    }

    return Status;
}

EFI_STATUS GetRSDPTR(IN EFI_SYSTEM_TABLE *SystemTable,
                     OUT BootInfoHead *Info)
{
    // 从 UEFI 系统表里找到 RSDP
    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++)
    {
        EFI_GUID acpi_2_0_guid = {0x8868e871, 0xe4f1, 0x11d3, {0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81}};
        // EFI_GUID acpi_1_0_guid = {0xeb9d2d30, 0x2d88, 0x11d3, {0x9a, 0x16, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}};
        if (CompareGuid(&SystemTable->ConfigurationTable[i].VendorGuid, &acpi_2_0_guid))
        {
            Info->BootAcpiPhysicalAddress =
                (EFI_PHYSICAL_ADDRESS)SystemTable->ConfigurationTable[i].VendorTable;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}