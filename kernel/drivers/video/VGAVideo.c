#include <datatype.h>
#include <gmos.h>
#include <drivers/video/VGAVideo.h>
#include <drivers/video/Font.h>

void VGAVideoInit(uint64_t FrameBufferBase,
                  uint64_t FrameBufferSize)
{
    for (uint64_t i = FrameBufferBase + SYSTEMPAGETABLEDIRECTMAPPING; i < FrameBufferBase + FrameBufferSize + SYSTEMPAGETABLEDIRECTMAPPING; i += 4)
    {
        uint32_t *pixel = (uint32_t *)i;
        *pixel = VGA_COLOR_BLACK;
    }
}

void SystemGlobalConsoleInfoInit(VGAVideoInfo VGA_Info,
                                 SystemGlobalConsole *Info,
                                 uint64_t PerPixelX,
                                 uint64_t PerPixelY)
{
    Info->PerPixelCharWidth = PerPixelX;
    Info->PerPixelCharHigth = PerPixelY;
    Info->AllPixelX = VGA_Info.HorizontalResolution / PerPixelX;
    Info->AllPixelY = VGA_Info.VerticalResolution / PerPixelY;
    Info->NowX = 0;
    Info->NowY = 0;
    Info->FrameBufferBase = VGA_Info.FrameBufferBase;
    Info->FrameBufferSize = VGA_Info.FrameBufferSize;
    Info->HorizontalResolution = VGA_Info.HorizontalResolution;
    Info->VerticalResolution = VGA_Info.VerticalResolution;
    Info->PixelsPerScanLine = VGA_Info.PixelsPerScanLine;
    Info->PixelFormat = VGA_Info.PixelFormat;
}

void VGAVideoDrawHLine(uint64_t FrameBufferBase,
                       uint64_t X,
                       uint64_t Y,
                       uint64_t Length,
                       VGAColor Color,
                       uint64_t m)
{
    // uint64_t Width = System_Info.HorizontalResolution;
    // uint64_t Hight = System_Info.VerticalResolution;
    // uint64_t RealPiexlsPerLine = System_Info.PixelsPerScanLine;
    uint64_t Offset = (Y * m + X) * 4;
    uint64_t start = FrameBufferBase + Offset;
    for (uint64_t i = start; i < start + Length * 4; i += 4)
    {
        uint32_t *pixel = (uint32_t *)i;
        *pixel = Color;
    }
}

void VGAVideoVLine(VGAVideoInfo System_Info,
                   uint64_t X,
                   uint64_t Y,
                   uint64_t Length,
                   VGAColor Color)
{
}

void PrintVirtualTextChar(SystemGlobalConsole *Info, uint8_t Ascii_Number)
{
    uint64_t NowX = Info->NowX;
    uint64_t NowY = Info->NowY;
    uint64_t Offset = 0;
    Offset = NowY * Info->PerPixelCharHigth * Info->PixelsPerScanLine * 4;
    Offset += NowX * Info->PerPixelCharWidth * 4;
    uint64_t pixelBaseAddress = Info->FrameBufferBase + Offset;
    for (uint64_t i = 0; i < Info->PerPixelCharHigth; i++)
    {
        uint32_t *piexl = (uint32_t *)(pixelBaseAddress + i * Info->PixelsPerScanLine * 4);
        for (uint64_t j = 0; j < Info->PerPixelCharWidth; j++, piexl++)
        {
            if ((ASCII_FONT[Ascii_Number][i] >> j) & 0x01)
            {
                *piexl = VGA_COLOR_GRAY;
            }
        }
    }
    Info->NowX++;
    if (Info->NowX > Info->AllPixelX)
    {
        Info->NowX = 0;
        Info->NowY++;
    }
}

void PrintVirtualTextChinaChar(SystemGlobalConsole Info, uint64_t ChinaChar_Number)
{
}