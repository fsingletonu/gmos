#ifndef _CPUMODE_H_
#define _CPUMODE_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

typedef enum
{
    CpuModeReal = 16,
    CpuModeProtected = 31,
    CpuModeProtectedPAE = 33,
    CpuModeLong = 64,
    CpuModeUnknown = 0
} CpuMode;

CpuMode GetCpuMode();

#endif
