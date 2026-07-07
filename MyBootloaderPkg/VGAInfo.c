#include "Include/VGAInfo.h"

EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

EFI_STATUS GetVGAInfo(IN EFI_SYSTEM_TABLE *SystemTable,
                      OUT BootInfoHead *Info)
{
    EFI_STATUS Status;

    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;

    Status = SystemTable->BootServices->LocateProtocol(&gopGuid,
                                                       NULL,
                                                       (void **)&Gop);

    if (EFI_ERROR(Status))
    {
        Print(L"Locate GOP Failed\r\n");
        return Status;
    }

    Info->VGAInfo.FrameBufferBase = Gop->Mode->FrameBufferBase;
    Info->VGAInfo.FrameBufferSize = Gop->Mode->FrameBufferSize;
    Info->VGAInfo.HorizontalResolution = Gop->Mode->Info->HorizontalResolution;
    Info->VGAInfo.PixelFormat = Gop->Mode->Info->PixelFormat;
    Info->VGAInfo.PixelsPerScanLine = Gop->Mode->Info->PixelsPerScanLine;
    Info->VGAInfo.VerticalResolution = Gop->Mode->Info->VerticalResolution;

    return EFI_SUCCESS;
}