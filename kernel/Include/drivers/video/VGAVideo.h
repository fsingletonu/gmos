#ifndef _VGAVIDEO_H_
#define _VGAVIDEO_H_

#include <datatype.h>
#include <drivers/video/Font.h>

#define CHAR_WIDTH 8
#define CHAR_HIGTH 16

typedef enum
{
    VGA_COLOR_BLACK = 0x00000000,
    VGA_COLOR_WHITE = 0x00ffffff,
    VGA_COLOR_BULE = 0x000000ff,
    VGA_COLOR_GREEN = 0x0000ff00,
    VGA_COLOR_RED = 0x00ff0000,
    VGA_COLOR_YELLOW = 0x00ffff00,
    VGA_COLOR_GRAY = 0x00c0c0c0
} VGAColor;

// 对物理机器的直接映射
typedef struct
{
    // 显卡基础信息
    uint64_t PixelFormat;
    uint64_t HorizontalResolution;
    uint64_t VerticalResolution;
    uint64_t FrameBufferBase;
    uint64_t FrameBufferSize;
    uint64_t PixelsPerScanLine;
} VGAVideoInfo;

// 想象出来的
typedef struct
{
    uint64_t AllPixelX;
    uint64_t AllPixelY;
    uint64_t PerPixelCharWidth;
    uint64_t PerPixelCharHigth;
    uint64_t NowX;
    uint64_t NowY;
    uint64_t FrameBufferBase;
    uint64_t FrameBufferSize;
    uint64_t HorizontalResolution;
    uint64_t VerticalResolution;
    uint64_t PixelsPerScanLine;
    uint64_t PixelFormat;
} SystemGlobalConsole;

void VGAVideoInit(uint64_t FrameBufferBase,
                  uint64_t FrameBufferSize);

void SystemGlobalConsoleInfoInit(VGAVideoInfo VGA_Info,
                                 SystemGlobalConsole *Info,
                                 uint64_t PerPixelX,
                                 uint64_t PerPixelY);

void VGAVideoDrawHLine(uint64_t FrameBufferBase,
                       uint64_t X,
                       uint64_t Y,
                       uint64_t Length,
                       VGAColor Color,
                       uint64_t m);

void VGAVideoVLine(VGAVideoInfo System_Info,
                   uint64_t X,
                   uint64_t Y,
                   uint64_t Length,
                   VGAColor Color);

void PrintVirtualTextChar(SystemGlobalConsole *Info,
                          uint8_t Ascii_Number);

void PrintVirtualTextChinaChar(SystemGlobalConsole Info,
                               uint64_t ChinaChar_Number);

#endif
