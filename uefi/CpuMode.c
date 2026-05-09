#include "Include/CpuMode.h"

CpuMode GetCpuMode()
{
    UINTN Cr0 = 0;
    UINTN Cr4 = 0;
    UINT64 Efer = 0;

    Cr0 = AsmReadCr0();
    Cr4 = AsmReadCr4();
    Efer = AsmReadMsr64(0xC0000080);

    if ((Cr0 & BIT0) == 0 && (Cr0 & BIT31) == 0 && (Cr4 & BIT5) == 0 && (Efer & BIT10) == 0)
    {
        return CpuModeReal;
    }
    if ((Cr0 & BIT0) && (Cr0 & BIT31) == 0 && (Cr4 & BIT5) == 0 && (Efer & BIT10) == 0)
    {
        return CpuModeProtected;
    }
    if ((Cr0 & BIT0) && (Cr0 & BIT31) && (Cr4 & BIT5) && (Efer & BIT10) == 0)
    {
        return CpuModeProtectedPAE;
    }
    if ((Cr0 & BIT0) && (Cr0 & BIT31) && (Cr4 & BIT5) && (Efer & BIT10))
    {
        return CpuModeLong;
    }

    return CpuModeUnknown;
}